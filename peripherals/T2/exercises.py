from utils import calcular_angulo, distancia_euclidiana

class ExerciseCounter:

    def __init__(self):
        self.contador    = 0
        self.invalidas   = 0
        self.estado      = "inicio"
        self.rep_invalida = False

    def _registrar_rep_valida(self):
        self.contador += 1
        self.rep_invalida = False

    def _registrar_rep_invalida(self):
        self.invalidas += 1
        self.rep_invalida = True

    def reset(self):
        self.contador     = 0
        self.invalidas    = 0
        self.estado       = "inicio"
        self.rep_invalida = False


class RoscaBiceps(ExerciseCounter): # VALIDADO

    LIMIAR_BAIXO  = 160
    LIMIAR_COMMIT = 120
    LIMIAR_CIMA   = 40

    def atualizar(self, pts):
        angulo = calcular_angulo(
            pts["ombro_esq"],
            pts["cotovelo_esq"],
            pts["pulso_esq"]
        )

        if self.estado == "inicio":
            if angulo > self.LIMIAR_BAIXO:
                self.estado = "pronto"

        elif self.estado == "pronto":
            if angulo < self.LIMIAR_COMMIT:
                self.estado = "subindo"

        elif self.estado == "subindo":
            if angulo < self.LIMIAR_CIMA:
                self.estado = "descendo"
                self._registrar_rep_valida()
            elif angulo > self.LIMIAR_BAIXO:
                self.estado = "pronto"
                self._registrar_rep_invalida()

        elif self.estado == "descendo":
            if angulo > self.LIMIAR_BAIXO:
                self.estado = "pronto"
                self.rep_invalida = False

        return self.contador, self.invalidas, self.rep_invalida, angulo


class Flexao(ExerciseCounter): # VALIDADO

    LIMIAR_CIMA   = 160
    LIMIAR_COMMIT = 130
    LIMIAR_BAIXO  = 90

    def atualizar(self, pts):
        angulo = calcular_angulo(
            pts["ombro_esq"],
            pts["cotovelo_esq"],
            pts["pulso_esq"]
        )

        if self.estado == "inicio":
            if angulo > self.LIMIAR_CIMA:
                self.estado = "pronto"

        elif self.estado == "pronto":
            if angulo < self.LIMIAR_COMMIT:
                self.estado = "descendo"

        elif self.estado == "descendo":
            if angulo < self.LIMIAR_BAIXO:
                self.estado = "subindo"
            elif angulo > self.LIMIAR_CIMA:
                self.estado = "pronto"
                self._registrar_rep_invalida()

        elif self.estado == "subindo":
            if angulo > self.LIMIAR_CIMA:
                self.estado = "pronto"
                self._registrar_rep_valida()

        return self.contador, self.invalidas, self.rep_invalida, angulo


class Agachamento(ExerciseCounter): # VALIDADO

    LIMIAR_CIMA   = 160
    LIMIAR_COMMIT = 140
    LIMIAR_BAIXO  = 95

    def atualizar(self, pts):
        angulo = calcular_angulo(
            pts["cintura_esq"],
            pts["joelho_esq"],
            pts["tornozelo_esq"]
        )

        if self.estado == "inicio":
            if angulo > self.LIMIAR_CIMA:
                self.estado = "pronto"

        elif self.estado == "pronto":
            if angulo < self.LIMIAR_COMMIT:
                self.estado = "descendo"

        elif self.estado == "descendo":
            if angulo < self.LIMIAR_BAIXO:
                self.estado = "subindo"
            elif angulo > self.LIMIAR_CIMA:
                self.estado = "pronto"
                self._registrar_rep_invalida()

        elif self.estado == "subindo":
            if angulo > self.LIMIAR_CIMA:
                self.estado = "pronto"
                self._registrar_rep_valida()

        return self.contador, self.invalidas, self.rep_invalida, angulo


class Abdominal(ExerciseCounter): # funciona cagado, precisa achar o angulo entre a diagonal do corpo e o quao dobrado ficam os joelhos e ficar em 135

    LIMIAR_DEITADO = 135
    LIMIAR_COMMIT  = 120
    LIMIAR_SENTADO = 110

    def atualizar(self, pts):
        angulo = calcular_angulo(
            pts["ombro_esq"],
            pts["cintura_esq"],
            pts["joelho_esq"]
        )

        if self.estado == "inicio":
            if angulo > self.LIMIAR_DEITADO:
                self.estado = "pronto"

        elif self.estado == "pronto":
            if angulo < self.LIMIAR_COMMIT:
                self.estado = "subindo"

        elif self.estado == "subindo":
            if angulo < self.LIMIAR_SENTADO:
                self.estado = "descendo"
                self._registrar_rep_valida()
            elif angulo > self.LIMIAR_DEITADO:
                self.estado = "pronto"
                self._registrar_rep_invalida()

        elif self.estado == "descendo":
            if angulo > self.LIMIAR_DEITADO:
                self.estado = "pronto"
                self.rep_invalida = False

        return self.contador, self.invalidas, self.rep_invalida, angulo

class Salto(ExerciseCounter): # VALIDADO

    CALIBRATION_FRAMES  = 30
    LIMIAR_SALTO_FATOR  = 0.15  
    LIMIAR_COMMIT_FATOR = 0.5   

    def __init__(self, altura_frame):
        super().__init__()
        self.estado               = "calibrando"
        self.altura_frame         = altura_frame
        self._calibration_readings = []
        self.baseline_y           = None
        self.limiar_salto         = int(altura_frame * self.LIMIAR_SALTO_FATOR)
        self.limiar_commit        = int(self.limiar_salto * self.LIMIAR_COMMIT_FATOR)

    def _hip_y(self, pts):
        return (pts["cintura_esq"][1] + pts["cintura_dir"][1]) // 2

    def atualizar(self, pts):
        y_atual = self._hip_y(pts)

        if self.estado == "calibrando":
            self._calibration_readings.append(y_atual)
            if len(self._calibration_readings) >= self.CALIBRATION_FRAMES:
                self.baseline_y = int(
                    sum(self._calibration_readings) / len(self._calibration_readings)
                )
                self.estado = "pronto"

        elif self.estado == "pronto":
            if y_atual < self.baseline_y - self.limiar_commit:
                self.estado = "no_ar"

        elif self.estado == "no_ar":
            if y_atual < self.baseline_y - self.limiar_salto:
                self._registrar_rep_valida()
                self.estado = "aterrissando"
            elif y_atual > self.baseline_y - self.limiar_commit:
                self._registrar_rep_invalida()
                self.estado = "pronto"

        elif self.estado == "aterrissando":
            if y_atual > self.baseline_y - self.limiar_commit:
                self.estado = "pronto"
                self.rep_invalida = False

        return self.contador, self.invalidas, self.rep_invalida, y_atual


class FlexaoLateralPerna(ExerciseCounter): # VALIDADO

    LIMIAR_BAIXO  = 90
    LIMIAR_COMMIT = 95
    LIMIAR_CIMA   = 110

    def _abducao_angulo(self, pts, lado):
        if lado == "esq":
            ref     = pts["cintura_dir"]
            quadril = pts["cintura_esq"]
            joelho  = pts["joelho_esq"]
        else:
            ref     = pts["cintura_esq"]
            quadril = pts["cintura_dir"]
            joelho  = pts["joelho_dir"]
        return calcular_angulo(ref, quadril, joelho)

    def _detectar_lado(self, pts):
        if pts["joelho_esq"][1] < pts["joelho_dir"][1]:
            return "esq"
        return "dir"

    def atualizar(self, pts):
        lado   = self._detectar_lado(pts)
        angulo = self._abducao_angulo(pts, lado)

        if self.estado == "inicio":
            if angulo < self.LIMIAR_BAIXO:
                self.estado = "pronto"

        elif self.estado == "pronto":
            if angulo > self.LIMIAR_COMMIT:
                self.estado = "subindo"

        elif self.estado == "subindo":
            if angulo > self.LIMIAR_CIMA:
                self.estado = "descendo"
                self._registrar_rep_valida()
            elif angulo < self.LIMIAR_BAIXO:
                self.estado = "pronto"
                self._registrar_rep_invalida()

        elif self.estado == "descendo":
            if angulo < self.LIMIAR_BAIXO:
                self.estado = "pronto"
                self.rep_invalida = False

        return self.contador, self.invalidas, self.rep_invalida, angulo
