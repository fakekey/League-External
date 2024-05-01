from Vippro import *
from utils.calc import *
from time import sleep

Vippro_script_info = {
    "script": "Viktor",
    "author": "nghilq",
    "description": "Viktor combo",
    "target_champ": "viktor",
}
e_range = 525.0
combo_key = 57
use_q_in_combo = True
use_e_in_combo = True
use_r_in_combo = True


def Vippro_load_cfg(cfg):
    global combo_key
    global use_q_in_combo, use_e_in_combo, use_r_in_combo

    combo_key = cfg.GetInt("combo_key", 57)
    use_q_in_combo = cfg.GetBool("use_q_in_combo", True)
    use_e_in_combo = cfg.GetBool("use_e_in_combo", True)
    use_r_in_combo = cfg.GetBool("use_r_in_combo", True)


def Vippro_save_cfg(cfg):
    global combo_key
    global use_q_in_combo, use_e_in_combo, use_r_in_combo

    cfg.SetInt("combo_key", combo_key)
    cfg.SetBool("use_q_in_combo", use_q_in_combo)
    cfg.SetBool("use_e_in_combo", use_e_in_combo)
    cfg.SetBool("use_r_in_combo", use_r_in_combo)


def Vippro_draw_settings(game, ui):
    global combo_key
    global use_q_in_combo, use_e_in_combo, use_r_in_combo

    combo_key = ui.KeySelect("Combo key", combo_key)

    if ui.TreeNode("Setting [Q]"):
        use_q_in_combo = ui.Checkbox("Use Q in Combo", use_q_in_combo)
        ui.TreePop()
    ui.Separator()
    if ui.TreeNode("Setting [E]"):
        use_e_in_combo = ui.Checkbox("Use E in Combo", use_e_in_combo)
        ui.TreePop()
    ui.Separator()
    if ui.TreeNode("Setting [R]"):
        use_r_in_combo = ui.Checkbox("Use R in Combo", use_r_in_combo)
        ui.TreePop()
    ui.Separator()


def combo(game):
    global use_q_in_combo, use_e_in_combo, use_r_in_combo

    me = game.player
    if me.isAlive and me.Q.IsReady(game.gameTime) and use_q_in_combo:
        target = get_best_target_in_range(me, game.champs, me.Q.info.castRange)
        if target:
            me.Q.MoveAndTrigger(game.WorldToScreen(target.position))
            return

    if me.isAlive and me.E.IsReady(game.gameTime) and use_e_in_combo:
        target = get_best_target_in_range(me, game.champs, e_range + e_range)
        if target:
            if me.position.distance(target.position) < e_range:
                prediction = predict_pos(game, target, 0.1)
                if prediction:
                    me.E.Cast(
                        game.WorldToScreen(target.position),
                        game.WorldToScreen(prediction),
                    )
            elif me.position.distance(target.position) < e_range + e_range:
                castStartPos = me.position.add(
                    target.position.sub(me.position).normalize().scale(e_range)
                )
                prediction = predict_pos(game, target, 0.1)
                if prediction:
                    me.E.Cast(
                        game.WorldToScreen(castStartPos), game.WorldToScreen(prediction)
                    )
    if (
        me.isAlive
        and me.R.IsReady(game.gameTime)
        and use_r_in_combo
        and me.R.name == "viktorchaosstorm"
    ):
        target = get_best_target_in_range(me, game.champs, me.R.info.castRange)
        if target:
            prediction = predict_pos(game, target, 0.1)
            if (
                prediction
                and me.position.distance_squared(prediction) <= me.R.info.castRange**2
            ):
                me.R.MoveAndTrigger(game.WorldToScreen(prediction))


def Vippro_update(game, ui):
    global combo_key
    global use_q_in_combo, use_e_in_combo, use_r_in_combo

    me = game.player
    if me.isAlive and not game.isChatOpen:
        if game.IsKeyDown(combo_key):
            combo(game)
