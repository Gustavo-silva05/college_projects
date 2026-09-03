import sys
import cv2
import time
import os
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from PyQt5 import uic

# ---------------------------------------------------------------------------
# Mapeamento exercício -> módulo de exercício e vídeo demonstrativo
# ---------------------------------------------------------------------------
EXERCISE_MAP = {
    "apoio":          {"class_name": "Flexao",             "video": "videos/apoio_demo.mp4"},
    "salto":          {"class_name": "Salto",              "video": "videos/salto_demo.mp4"},
    "agachamento":    {"class_name": "Agachamento",        "video": "videos/agachamento_demo.mp4"},
    "flexao_lateral": {"class_name": "FlexaoLateralPerna", "video": "videos/flexao_lateral_demo.mp4"},
    "rosca_biceps":   {"class_name": "RoscaBiceps",        "video": "videos/rosca_biceps_demo.mp4"},
    "abdominal":      {"class_name": "Abdominal",          "video": "videos/abdominal_demo.mp4"},
}

MODEL_PATH     = "pose_landmarker.task"
CAMERA_INDEX   = 0

# ---------------------------------------------------------------------------
# Thread de processamento: MediaPipe + exercício
# ---------------------------------------------------------------------------
class ExerciseThread(QThread):
    frame_ready    = pyqtSignal(QImage)          # frame anotado para area_camera
    stats_updated  = pyqtSignal(int, int, bool, float)  # contador, invalidas, rep_invalida, valor
    goal_reached   = pyqtSignal()

    def __init__(self, exercise_key: str, meta: int, camera_index: int = CAMERA_INDEX):
        super().__init__()
        self.exercise_key   = exercise_key
        self.meta           = meta
        self.camera_index   = camera_index
        self._running       = True

    def stop(self):
        self._running = False
        self.wait()

    def run(self):
        # Imports pesados ficam aqui para não bloquear a UI
        import mediapipe as mp
        from mediapipe.tasks import python
        from mediapipe.tasks.python import vision
        from mediapipe.tasks.python.vision import RunningMode
        import points_tasks as pts_module
        from exercises import (
            RoscaBiceps, Flexao, Agachamento,
            Abdominal, Salto, FlexaoLateralPerna,
        )

        CLASS_MAP = {
            "Flexao":             Flexao,
            "Salto":              Salto,
            "Agachamento":        Agachamento,
            "FlexaoLateralPerna": FlexaoLateralPerna,
            "RoscaBiceps":        RoscaBiceps,
            "Abdominal":          Abdominal,
        }

        # --- MediaPipe ---
        base_options = python.BaseOptions(model_asset_path=MODEL_PATH)
        options      = vision.PoseLandmarkerOptions(
            base_options=base_options,
            running_mode=RunningMode.VIDEO,
            num_poses=1,
            min_pose_detection_confidence=0.5,
            min_pose_presence_confidence=0.5,
            min_tracking_confidence=0.5,
        )
        landmarker = vision.PoseLandmarker.create_from_options(options)

        # --- Câmera ---
        captura = cv2.VideoCapture(self.camera_index)
        if not captura.isOpened():
            landmarker.close()
            return

        ret, frame_teste = captura.read()
        if not ret:
            captura.release()
            landmarker.close()
            return

        h_frame, w_frame = frame_teste.shape[:2]

        # --- Exercício ---
        class_name = EXERCISE_MAP[self.exercise_key]["class_name"]
        cls        = CLASS_MAP[class_name]
        # Salto precisa de altura_frame
        exercicio  = cls(altura_frame=h_frame) if class_name == "Salto" else cls()

        CONNECTIONS = [
            (11, 12), (11, 13), (13, 15),
            (12, 14), (14, 16),
            (11, 23), (12, 24),
            (23, 24),
            (23, 25), (25, 27),
            (24, 26), (26, 28),
        ]

        def desenhar_skeleton(img, landmarks, w, h):
            pts = [(int(lm.x * w), int(lm.y * h)) for lm in landmarks]
            for a, b in CONNECTIONS:
                cv2.line(img, pts[a], pts[b], (0, 255, 0), 2)
            for p in pts:
                cv2.circle(img, p, 4, (255, 255, 255), -1)

        FONTE        = cv2.FONT_HERSHEY_SIMPLEX
        COR_VALIDA   = (0, 255, 0)
        COR_INVALIDA = (0, 0, 255)

        def desenhar_hud(img, contador, invalidas, rep_invalida, info_valor, meta):
            overlay = img.copy()
            cv2.rectangle(overlay, (0, 0), (260, 140), (0, 0, 0), -1)
            cv2.addWeighted(overlay, 0.4, img, 0.6, 0, img)
            cor_borda = COR_INVALIDA if rep_invalida else COR_VALIDA
            cv2.putText(img, f"Reps: {contador} / {meta}",  (10, 35),  FONTE, 1.0, cor_borda, 2)
            cv2.putText(img, f"Invalidas: {invalidas}",      (10, 70),  FONTE, 0.8,
                        COR_INVALIDA if invalidas > 0 else COR_VALIDA, 2)
            cv2.putText(img, f"Valor: {info_valor:.1f}",     (10, 105), FONTE, 0.7, (255, 255, 255), 1)
            if rep_invalida:
                cv2.putText(img, "! REP INCOMPLETA !",
                            (w_frame // 2 - 160, h_frame - 20),
                            FONTE, 1.2, COR_INVALIDA, 3)

        # --- Countdown ---
        inicio = time.time()
        COUNTDOWN = 3
        while self._running:
            ret, imagem = captura.read()
            if not ret:
                break
            restante = COUNTDOWN - int(time.time() - inicio)
            if restante <= 0:
                break
            cv2.putText(imagem, "Entre em posicao!",
                        (w_frame // 2 - 160, h_frame // 2 - 50), FONTE, 1.0, (255, 255, 0), 2)
            cv2.putText(imagem, str(restante),
                        (w_frame // 2 - 30, h_frame // 2 + 40), FONTE, 3.0, (0, 255, 0), 4)
            self._emit_frame(imagem)
            self.msleep(50)

        # --- Loop principal ---
        frame_idx = 0
        while self._running and captura.isOpened():
            ret, imagem = captura.read()
            if not ret:
                break

            h, w = imagem.shape[:2]
            imagem_rgb = cv2.cvtColor(imagem, cv2.COLOR_BGR2RGB)
            mp_image   = mp.Image(image_format=mp.ImageFormat.SRGB, data=imagem_rgb)

            timestamp_ms = int(captura.get(cv2.CAP_PROP_POS_MSEC))
            if timestamp_ms == 0:
                timestamp_ms = frame_idx * 33
            resultado = landmarker.detect_for_video(mp_image, timestamp_ms)

            contador, invalidas, rep_invalida, info_valor = 0, 0, False, 0.0

            if resultado.pose_landmarks:
                landmarks = resultado.pose_landmarks[0]
                desenhar_skeleton(imagem, landmarks, w, h)
                pontos    = pts_module.get_points(landmarks, w, h)
                contador, invalidas, rep_invalida, info_valor = exercicio.atualizar(pontos)

            # desenhar_hud(imagem, contador, invalidas, rep_invalida, info_valor, self.meta)
            # self.stats_updated.emit(contador, invalidas, rep_invalida, info_valor)
            self._emit_frame(imagem)

            if contador >= self.meta:
                # Mostra "META ATINGIDA" por 2s
                cv2.putText(imagem, "META ATINGIDA!",
                            (w // 2 - 140, h // 2), FONTE, 1.5, COR_VALIDA, 3)
                self._emit_frame(imagem)
                self.msleep(2000)
                self.goal_reached.emit()
                break

            frame_idx += 1

        captura.release()
        landmarker.close()

    def _emit_frame(self, frame_bgr):
        h, w, ch = frame_bgr.shape
        bpl       = ch * w
        image     = QImage(frame_bgr.data, w, h, bpl, QImage.Format_RGB888).rgbSwapped()
        self.frame_ready.emit(image)


# ---------------------------------------------------------------------------
# Player de vídeo demonstrativo (roda em QThread, exibe na area_camera)
# ---------------------------------------------------------------------------
class DemoVideoThread(QThread):
    frame_ready = pyqtSignal(QImage)

    def __init__(self, video_path: str):
        super().__init__()
        self.video_path = video_path
        self._running   = True

    def stop(self):
        self._running = False
        self.wait()

    def run(self):
        if not os.path.exists(self.video_path):
            return

        cap = cv2.VideoCapture(self.video_path)
        fps = cap.get(cv2.CAP_PROP_FPS) or 30
        delay = int(1000 / fps)

        while self._running and cap.isOpened():
            ret, frame = cap.read()
            if not ret:
                # loop do vídeo
                cap.set(cv2.CAP_PROP_POS_FRAMES, 0)
                continue

            h, w, ch = frame.shape
            bpl       = ch * w
            image     = QImage(frame.data, w, h, bpl, QImage.Format_RGB888).rgbSwapped()
            self.frame_ready.emit(image)
            self.msleep(delay)

        cap.release()


# ---------------------------------------------------------------------------
# Janela principal
# ---------------------------------------------------------------------------
class Window(QDialog):
    def __init__(self):
        super().__init__()
        uic.loadUi("interface.ui", self)
        self.UiComponents()

        self._exercise_thread: ExerciseThread | None = None
        self._demo_thread:     DemoVideoThread | None = None

        # Exercício ativo padrão (primeiro radio marcado)
        self._current_exercise = self._get_selected_exercise()
        self._start_demo_video()

        self.show()

    # ------------------------------------------------------------------
    # Helpers
    # ------------------------------------------------------------------
    def _get_selected_exercise(self) -> str:
        for key, rb in self._radio_buttons.items():
            if rb.isChecked():
                return key
        return "apoio"

    def _get_meta(self) -> int:
        try:
            v = int(self.reps_edit.text())
            return max(1, v)
        except ValueError:
            return 10

    # ------------------------------------------------------------------
    # Demo video
    # ------------------------------------------------------------------
    @pyqtSlot(QImage)
    def _update_demo_label(self, image: QImage):
        pixmap = QPixmap.fromImage(image).scaled(
            self.area_como_fazer.size(),
            Qt.KeepAspectRatio,
            Qt.SmoothTransformation,
        )
        self.area_como_fazer.setPixmap(pixmap)
    def _start_demo_video(self):
        self._stop_demo_video()
        video_path = EXERCISE_MAP[self._current_exercise]["video"]
        self._demo_thread = DemoVideoThread(video_path)
        self._demo_thread.frame_ready.connect(self._update_demo_label)
        self._demo_thread.start()

    def _stop_demo_video(self):
        if self._demo_thread and self._demo_thread.isRunning():
            self._demo_thread.stop()
        self._demo_thread = None

    # ------------------------------------------------------------------
    # Exercise thread
    # ------------------------------------------------------------------
    def _start_exercise(self):
        self._stop_exercise()
        # self._stop_demo_video()
        self.lcd_reps_counter.display(0)
        self.invalid_reps.setText("Invalidas: 0")
        thread = ExerciseThread(
            exercise_key=self._current_exercise,
            meta=self._get_meta(),
        )
        self.target_reps.setText(f"Meta: {self._get_meta()} reps")
        thread.frame_ready.connect(self._update_camera_label)
        thread.stats_updated.connect(self._on_stats_updated)
        thread.goal_reached.connect(self._on_goal_reached)
        thread.finished.connect(self._on_thread_finished)
        self._exercise_thread = thread
        self._exercise_thread.start()

    def _stop_exercise(self):
        if self._exercise_thread and self._exercise_thread.isRunning():
            self._exercise_thread.stop()
        self._exercise_thread = None

    # ------------------------------------------------------------------
    # Slots
    # ------------------------------------------------------------------
    @pyqtSlot(QImage)
    def _update_camera_label(self, image: QImage):
        pixmap = QPixmap.fromImage(image).scaled(
            self.area_camera.size(),
            Qt.KeepAspectRatio,
            Qt.SmoothTransformation,
        )
        self.area_camera.setPixmap(pixmap)

    @pyqtSlot(int, int, bool, float)
    def _on_stats_updated(self, contador, invalidas, rep_invalida, valor):
        self.lcd_reps_counter.display(contador)
        self.invalid_reps.setText(f"Invalidas: {invalidas}")
        if rep_invalida:
            self.invalid_reps.setStyleSheet("color: red;")
        else:
            self.invalid_reps.setStyleSheet("color: black;")

    @pyqtSlot()
    def _on_goal_reached(self):
        self._stop_exercise()
        self._start_demo_video()

    @pyqtSlot()
    def _on_thread_finished(self):
        # Se o exercício terminou sem atingir meta (usuário parou), volta demo
        if self._demo_thread is None:
            self._start_demo_video()

    def _on_exercise_changed(self, key: str):
        """Chamado quando qualquer radio button é selecionado."""
        self._current_exercise = key
        was_running = self._exercise_thread and self._exercise_thread.isRunning()
        self._stop_exercise()
        self._stop_demo_video()
        self.lcd_reps_counter.display(0)
        self._start_exercise()
        self._start_demo_video()

    def funcao_ligacam(self):
        self._start_exercise()

    def funcao_fechacam(self):
        self._stop_exercise()
        self.area_camera.clear()
        self._start_demo_video()

    def funcao_sair(self):
        self._stop_exercise()
        self._stop_demo_video()
        self.close()

    def funcao_salvaframe(self):
        # Captura o pixmap atual da label
        pixmap = self.area_camera.pixmap()
        if pixmap:
            pixmap.save("foto.png")

    # ------------------------------------------------------------------
    # UI wiring
    # ------------------------------------------------------------------
    def UiComponents(self):
        self._radio_buttons: dict[str, QRadioButton] = {
            "apoio":          self.findChild(QRadioButton, "apoio"),
            "salto":          self.findChild(QRadioButton, "salto"),
            "agachamento":    self.findChild(QRadioButton, "agachamento"),
            "flexao_lateral": self.findChild(QRadioButton, "flexao_lateral"),
            "rosca_biceps":   self.findChild(QRadioButton, "rosca_biceps"),
            "abdominal":      self.findChild(QRadioButton, "abdominal"),
        }
        
        self._radio_buttons["apoio"].setChecked(True)  # Exercício padrão

        self.title           = self.findChild(QLabel,     "title")
        self.area_camera     = self.findChild(QLabel,     "area_camera")
        self.area_como_fazer = self.findChild(QLabel,     "area_como_faz")
        self.target_reps     = self.findChild(QLabel,     "target_reps")   
        self.invalid_reps    = self.findChild(QLabel,     "invalid_reps")
        self.lcd_reps_counter= self.findChild(QLCDNumber, "lcd_reps_counter")
        self.reps_edit       = self.findChild(QLineEdit,  "reps_edit")
        self.start_button    = self.findChild(QPushButton, "start_button")
        self.stop_button     = self.findChild(QPushButton, "stop_button")
        self.set_button      = self.findChild(QPushButton, "set_button")
        
        self.target_reps.setText(f"Meta: {self._get_meta()} reps")
        self.invalid_reps.setText("Invalidas: 0")

        # Radio buttons — cria funções lambda para capturar a key corretamente
        for key, rb in self._radio_buttons.items():
            if rb:
                rb.toggled.connect(
                    lambda checked, k=key: self._on_exercise_changed(k) if checked else None
                )

        self.start_button.clicked.connect(self.funcao_ligacam)
        self.stop_button.clicked.connect(self.funcao_fechacam)
        self.set_button.clicked.connect(self._start_exercise)

    def closeEvent(self, event):
        self._stop_exercise()
        self._stop_demo_video()
        event.accept()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = Window()
    with open("style.css", "r", encoding="utf-8") as f:
        app.setStyleSheet(f.read())
    app.exec_()