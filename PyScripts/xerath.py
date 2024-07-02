from Vippro import *
from utils.calc import *
from utils.input import *
from time import sleep
from orbwalking import resetAtk

Vippro_script_info = {
    "script": "Xerath",
    "author": "nghilq",
    "description": "Xerath combo",
    "target_champ": "xerath",
}

combo_key = 57
r_key = 2
use_q_in_combo = True
use_e_in_combo = True
use_w_in_combo = True


def Vippro_load_cfg(cfg):
    global combo_key, r_key
    global use_q_in_combo, use_e_in_combo, use_w_in_combo

    combo_key = cfg.GetInt("combo_key", 57)
    r_key = cfg.GetInt("r_key", 2)
    use_q_in_combo = cfg.GetBool("use_q_in_combo", True)
    use_e_in_combo = cfg.GetBool("use_e_in_combo", True)
    use_w_in_combo = cfg.GetBool("use_w_in_combo", True)


def Vippro_save_cfg(cfg):
    global combo_key, r_key
    global use_q_in_combo, use_e_in_combo, use_w_in_combo

    cfg.SetInt("combo_key", combo_key)
    cfg.SetInt("r_key", r_key)
    cfg.SetBool("use_q_in_combo", use_q_in_combo)
    cfg.SetBool("use_e_in_combo", use_e_in_combo)
    cfg.SetBool("use_w_in_combo", use_w_in_combo)


def Vippro_draw_settings(game, ui):
    global combo_key, r_key
    global use_q_in_combo, use_e_in_combo, use_w_in_combo

    combo_key = ui.KeySelect("Combo key", combo_key)
    r_key = ui.KeySelect("R key", r_key)

    if ui.TreeNode("Setting [Q]"):
        use_q_in_combo = ui.Checkbox("Use Q in Combo", use_q_in_combo)
        ui.TreePop()
    ui.Separator()
    if ui.TreeNode("Setting [W]"):
        use_w_in_combo = ui.Checkbox("Use W in Combo", use_w_in_combo)
        ui.TreePop()
    ui.Separator()
    if ui.TreeNode("Setting [E]"):
        use_e_in_combo = ui.Checkbox("Use E in Combo", use_e_in_combo)
        ui.TreePop()
    ui.Separator()


charge_start_time = 0.0


def charge_q(me, game_time):
    global charge_start_time
    Trigger("q", True)
    charge_start_time = game_time


def q_range(charge_time: float):
    if charge_time <= 0.4:
        return 200
    if charge_time >= 1:
        return 1450
    return 735 + 102.14 * (charge_time - 0.25) * 4


def castingQ(me):
    buff = me.GetBuffByName("XerathArcanopulseChargeUp")
    if buff and buff.countAlt > 0:
        return True

    return False


def combo(game):
    global use_q_in_combo, use_e_in_combo, use_w_in_combo
    global charging_q, charge_start_time

    me = game.player
    if me.isAlive and me.Q.IsReady(game.gameTime) and use_q_in_combo:
        target = get_best_target_in_range(me, game.champs, me.Q.info.castRange)
        if target:
            if not castingQ(me):
                charge_q(me, game.gameTime)
                return
            current_charge_time = game.gameTime - charge_start_time
            current_q_range = q_range(current_charge_time)
            pos = predict_pos(game, target, 0.528)
            if me.position.distance(pos) <= current_q_range - 80:
                if castingQ(me):
                    MoveAndRelease("q", game.WorldToScreen(pos))
                    resetAtk()

    if me.isAlive and me.W.IsReady(game.gameTime) and use_w_in_combo:
        target = get_best_target_in_range(me, game.champs, me.W.info.castRange)
        if target:
            pos = predict_pos(game, target, (1000 / 9999999) + 0.3)
            if me.position.distance(pos) <= 1100:
                MoveAndTrigger("w", game.WorldToScreen(pos))
                resetAtk()

    if me.isAlive and me.E.IsReady(game.gameTime) and use_e_in_combo:
        target = get_best_target_in_range(me, game.champs, me.E.info.castRange)
        if target:
            pos = predict_pos(
                game, target, (me.position.distance(target.position) / 1600.0) + 0.1
            )
            if me.position.distance(pos) <= 950 and not is_collisioned(
                game, target, "minion", me.E.info.width
            ):
                MoveAndTrigger("e", game.WorldToScreen(pos))
                resetAtk()


def r_trigger(game):
    me = game.player

    if me.isAlive and me.R.IsReady(game.gameTime):
        target = get_best_target_in_range(me, game.champs, 5000)
        if target:
            pos = predict_pos(game, target, 0.26)
            if pos and me.position.distance_squared(pos) <= 5000**2:
                MoveAndTrigger("r", game.WorldToScreen(pos))
                resetAtk()


def Vippro_update(game, ui):
    global combo_key, r_key
    global use_q_in_combo, use_e_in_combo, use_w_in_combo

    me = game.player
    if me.isAlive and not game.isChatOpen:
        if game.IsKeyDown(combo_key):
            combo(game)
        if game.IsKeyDown(r_key):
            r_trigger(game)
