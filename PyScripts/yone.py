from Vippro import *
from utils.calc import *
from time import sleep

Vippro_script_info = {
    "script": "Yone",
    "author": "nghilq",
    "description": "Yone combo",
    "target_champ": "yone",
}

combo_key = 57
r_key = 2
use_q12_in_combo = True
use_q3_in_combo = True
use_w_in_combo = True


def Vippro_load_cfg(cfg):
    global combo_key, r_key
    global use_q12_in_combo, use_q3_in_combo, use_w_in_combo

    combo_key = cfg.GetInt("combo_key", 57)
    r_key = cfg.GetInt("r_key", 2)
    use_q12_in_combo = cfg.GetBool("use_q12_in_combo", True)
    use_q3_in_combo = cfg.GetBool("use_q3_in_combo", True)
    use_w_in_combo = cfg.GetBool("use_w_in_combo", True)


def Vippro_save_cfg(cfg):
    global combo_key, r_key
    global use_q12_in_combo, use_q3_in_combo, use_w_in_combo

    cfg.SetInt("combo_key", combo_key)
    cfg.SetInt("r_key", r_key)
    cfg.SetBool("use_q12_in_combo", use_q12_in_combo)
    cfg.SetBool("use_q3_in_combo", use_q3_in_combo)
    cfg.SetBool("use_w_in_combo", use_w_in_combo)


def Vippro_draw_settings(game, ui):
    global combo_key, r_key
    global use_q12_in_combo, use_q3_in_combo, use_w_in_combo

    combo_key = ui.KeySelect("Combo key", combo_key)
    r_key = ui.KeySelect("R key", r_key)

    if ui.TreeNode("Setting [Q]"):
        use_q12_in_combo = ui.Checkbox("Use Q1/Q2 in Combo", use_q12_in_combo)
        use_q3_in_combo = ui.Checkbox("Use Q3 in Combo", use_q3_in_combo)
        ui.TreePop()
    ui.Separator()
    if ui.TreeNode("Setting [W]"):
        use_w_in_combo = ui.Checkbox("Use W in Combo", use_w_in_combo)
        ui.TreePop()
    ui.Separator()


def combo(game):
    global use_q12_in_combo, use_q3_in_combo, use_w_in_combo

    me = game.player
    if (
        me.isAlive
        and me.Q.IsReady(game.gameTime)
        and use_q12_in_combo
        and me.Q.name == "yoneq"
    ):
        target = get_best_target_in_range(me, game.champs, 450)
        if target:
            pos = predict_pos(game, target, 450 / 1500)
            if pos and me.position.distance_squared(pos) <= 450**2:
                me.Q.MoveAndTrigger(game.WorldToScreen(pos))
                return

    if (
        me.isAlive
        and me.Q.IsReady(game.gameTime)
        and use_q3_in_combo
        and me.Q.name == "yoneq3"
    ):
        target = get_best_target_in_range(me, game.champs, 1050)
        if target:
            pos = predict_pos(game, target, 1050 / 1500)
            if pos and me.position.distance_squared(pos) <= 1050**2:
                me.Q.MoveAndTrigger(game.WorldToScreen(pos))
                return
    if me.isAlive and me.W.IsReady(game.gameTime) and use_w_in_combo:
        target = get_best_target_in_range(me, game.champs, 700)
        if target:
            pos = predict_pos(game, target, 700 / 1800)
            if pos and me.position.distance_squared(pos) <= 700**2:
                me.W.MoveAndTrigger(game.WorldToScreen(pos))
                return


def r_trigger(game):
    me = game.player

    if me.isAlive and me.R.IsReady(game.gameTime):
        target = get_best_target_in_range(me, game.champs, 1000)
        if target:
            pos = predict_pos(game, target, 1000 / 1500)
            if pos and me.position.distance_squared(pos) <= 950**2:
                me.R.MoveAndTrigger(game.WorldToScreen(pos))
                return


def Vippro_update(game, ui):
    global combo_key, r_key
    global use_q12_in_combo, use_q3_in_combo, use_w_in_combo

    me = game.player
    if me.isAlive and not game.isChatOpen:
        if game.IsKeyDown(combo_key):
            combo(game)
        if game.IsKeyDown(r_key):
            r_trigger(game)
