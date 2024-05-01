from Vippro import *
from utils.calc import *
from time import sleep

Vippro_script_info = {
    "script": "Yasuo",
    "author": "nghilq",
    "description": "Yasuo autoQ",
    "target_champ": "yasuo",
}

combo_key = 57
use_q12_in_combo = True
use_q3_in_combo = True


def Vippro_load_cfg(cfg):
    global combo_key
    global use_q12_in_combo, use_q3_in_combo

    combo_key = cfg.GetInt("combo_key", 57)
    use_q12_in_combo = cfg.GetBool("use_q12_in_combo", True)
    use_q3_in_combo = cfg.GetBool("use_q3_in_combo", True)


def Vippro_save_cfg(cfg):
    global combo_key
    global use_q12_in_combo, use_q3_in_combo

    cfg.SetInt("combo_key", combo_key)
    cfg.SetBool("use_q12_in_combo", use_q12_in_combo)
    cfg.SetBool("use_q3_in_combo", use_q3_in_combo)


def Vippro_draw_settings(game, ui):
    global combo_key
    global use_q12_in_combo, use_q3_in_combo

    combo_key = ui.KeySelect("Combo key", combo_key)

    if ui.TreeNode("Setting [Q]"):
        use_q12_in_combo = ui.Checkbox("Use Q1/Q2 in Combo", use_q12_in_combo)
        use_q3_in_combo = ui.Checkbox("Use Q3 in Combo", use_q3_in_combo)
        ui.TreePop()
    ui.Separator()


def combo(game):
    global use_q12_in_combo, use_q3_in_combo

    me = game.player
    if (
        me.isAlive
        and me.Q.IsReady(game.gameTime)
        and use_q12_in_combo
        and me.Q.name != "yasuoq3wrapper"
    ):
        target = get_best_target_in_range(me, game.champs, 475)
        if target:
            pos = predict_pos(game, target, 475 / 1500)
            if pos and me.position.distance_squared(pos) <= 475**2:
                me.Q.MoveAndTrigger(game.WorldToScreen(pos))
                return

    if (
        me.isAlive
        and me.Q.IsReady(game.gameTime)
        and use_q3_in_combo
        and me.Q.name == "yasuoq3wrapper"
    ):
        target = get_best_target_in_range(me, game.champs, 1000)
        if target:
            pos = predict_pos(game, target, 1000 / 1500)
            if pos and me.position.distance_squared(pos) <= 1000**2:
                me.Q.MoveAndTrigger(game.WorldToScreen(pos))
                return


def Vippro_update(game, ui):
    global combo_key
    global use_q12_in_combo, use_q3_in_combo

    me = game.player
    if me.isAlive and not game.isChatOpen:
        if game.IsKeyDown(combo_key):
            combo(game)
