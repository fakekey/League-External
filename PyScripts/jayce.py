from Vippro import *
from utils.calc import *
from time import sleep

Vippro_script_info = {
    "script": "Jayce",
    "author": "nghilq",
    "description": "5ty combo",
    "target_champ": "jayce",
}

qe_key = 2
free_qe_key = 3


def Vippro_load_cfg(cfg):
    global qe_key, free_qe_key

    qe_key = cfg.GetInt("qe_key", 2)
    free_qe_key = cfg.GetInt("free_qe_key", 3)


def Vippro_save_cfg(cfg):
    global qe_key, free_qe_key

    cfg.SetInt("qe_key", qe_key)
    cfg.SetInt("free_qe_key", free_qe_key)


def Vippro_draw_settings(game, ui):
    pass


def QECombo(game, me):
    if me.Q.IsReady(game.gameTime) and me.E.IsReady(game.gameTime) and me.Q.name == "jayceshockblast" and me.E.name == "jayceaccelerationgate":
        target = get_best_target_in_range(me, game.champs, 1750)
        if target:
            predict = predict_pos(game, target, 0.1)
            if (me.position.distance_squared(predict) <= pow(1750, 2)):
                me.Q.MoveAndTrigger(game.WorldToScreen(predict))
                sleep(0.005)
                me.E.MoveAndTrigger(game.WorldToScreen(me.position))
                sleep(0.1)


def FreeQECombo(game, me):
    if me.Q.IsReady(game.gameTime) and me.E.IsReady(game.gameTime) and me.Q.name == "jayceshockblast" and me.E.name == "jayceaccelerationgate":
        me.Q.MoveAndTrigger(game.GetCursor())
        sleep(0.005)
        me.E.MoveAndTrigger(game.WorldToScreen(me.position))
        sleep(0.1)


def Vippro_update(game, ui):
    global qe_key, free_qe_key

    me = game.player
    if me.isAlive and not game.isChatOpen:
        if game.IsKeyDown(qe_key):
            QECombo(game, me)
        if game.IsKeyDown(free_qe_key):
            FreeQECombo(game, me)
