def get_points(landmarks, w, h):

    def px(idx):
        lm = landmarks[idx]
        return (int(lm.x * w), int(lm.y * h))

    LEFT_SHOULDER  = 11
    RIGHT_SHOULDER = 12
    LEFT_ELBOW     = 13
    RIGHT_ELBOW    = 14
    LEFT_WRIST     = 15
    RIGHT_WRIST    = 16
    LEFT_HIP       = 23
    RIGHT_HIP      = 24
    LEFT_KNEE      = 25
    RIGHT_KNEE     = 26
    LEFT_ANKLE     = 27
    RIGHT_ANKLE    = 28

    return {
        "ombro_esq":     px(LEFT_SHOULDER),
        "ombro_dir":     px(RIGHT_SHOULDER),
        "cotovelo_esq":  px(LEFT_ELBOW),
        "cotovelo_dir":  px(RIGHT_ELBOW),
        "pulso_esq":     px(LEFT_WRIST),
        "pulso_dir":     px(RIGHT_WRIST),
        "cintura_esq":   px(LEFT_HIP),
        "cintura_dir":   px(RIGHT_HIP),
        "joelho_esq":    px(LEFT_KNEE),
        "joelho_dir":    px(RIGHT_KNEE),
        "tornozelo_esq": px(LEFT_ANKLE),
        "tornozelo_dir": px(RIGHT_ANKLE),
    }
