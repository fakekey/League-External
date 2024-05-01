from Vippro import *
from utils.calc import *
from time import sleep

Vippro_script_info = {
    "script": "Twistedfate",
    "author": "nghilq",
    "description": "Chau Nhuan Phat combo",
    "target_champ": "twistedfate",
}

combo_key = 57
blue_key = 2
red_key = 3
gold_key = 4


use_q_in_combo = True
use_w_in_combo = True


def Vippro_load_cfg(cfg):
    global combo_key, blue_key, red_key, gold_key
    global use_q_in_combo, use_w_in_combo, combo_mode

    combo_key = cfg.GetInt("combo_key", 57)
    blue_key = cfg.GetInt("blue_key", 2)
    red_key = cfg.GetInt("red_key", 3)
    gold_key = cfg.GetInt("gold_key", 4)
    combo_mode = cfg.GetInt("combo_mode", 0)
    use_q_in_combo = cfg.GetBool("use_q_in_combo", True)
    use_w_in_combo = cfg.GetBool("use_w_in_combo", True)


def Vippro_save_cfg(cfg):
    global combo_key, blue_key, red_key, gold_key
    global use_q_in_combo, use_w_in_combo, combo_mode

    cfg.SetInt("combo_key", combo_key)
    cfg.SetInt("blue_key", blue_key)
    cfg.SetInt("red_key", red_key)
    cfg.SetInt("gold_key", gold_key)
    cfg.SetInt("combo_mode", combo_mode)
    cfg.SetBool("use_q_in_combo", use_q_in_combo)
    cfg.SetBool("use_w_in_combo", use_w_in_combo)


def Vippro_draw_settings(game, ui):
    global combo_key, blue_key, red_key, gold_key
    global use_q_in_combo, use_w_in_combo, combo_mode

    combo_key = ui.KeySelect("Combo key", combo_key)
    blue_key = ui.KeySelect("Blue Card key (Hold)", blue_key)
    red_key = ui.KeySelect("Red Card key (Hold)", red_key)
    gold_key = ui.KeySelect("Gold Card key (Hold)", gold_key)

    if ui.TreeNode("Setting [Q]"):
        use_q_in_combo = ui.Checkbox("Use Q in Combo", use_q_in_combo)
        ui.TreePop()
    ui.Separator()
    if ui.TreeNode("Setting [W]"):
        use_w_in_combo = ui.Checkbox("Use W in Combo", use_w_in_combo)
        combo_mode = ui.ListBox(
            "Auto Card in Combo", ["Gold Card", "Red Card", "Blue Card"], combo_mode
        )
        ui.TreePop()
    ui.Separator()


def keyBlue(game, me):
    if me.isAlive and me.W.IsReady(game.gameTime):
        if me.W.name == "pickacard":
            me.W.Trigger(True)
        if me.W.name == "bluecardlock":
            me.W.Trigger(False)


def keyRed(game, me):
    if me.isAlive and me.W.IsReady(game.gameTime):
        if me.W.name == "pickacard":
            me.W.Trigger(True)
        if me.W.name == "redcardlock":
            me.W.Trigger(False)


def keyGold(game, me):
    if me.isAlive and me.W.IsReady(game.gameTime):
        if me.W.name == "pickacard":
            me.W.Trigger(True)
        if me.W.name == "goldcardlock":
            me.W.Trigger(False)


def combo(game, me):
    global use_q_in_combo, use_w_in_combo, combo_mode

    if me.isAlive and me.Q.IsReady(game.gameTime) and use_q_in_combo:
        target = get_best_target_in_range(me, game.champs, 1450)
        if target:
            pos = predict_pos(game, target, 1450 / 10000)
            if pos and me.position.distance_squared(pos) <= 1300**2:
                me.Q.MoveAndTrigger(game.WorldToScreen(pos))
                return

    if me.isAlive and me.W.IsReady(game.gameTime) and use_w_in_combo:
        target = get_best_target_in_range(me, game.champs)
        if target:
            if combo_mode == 0:
                keyGold(game, me)
            elif combo_mode == 1:
                keyRed(game, me)
            elif combo_mode == 2:
                keyBlue(game, me)


def Vippro_update(game, ui):
    global combo_key, blue_key, red_key, gold_key
    global use_q_in_combo, use_w_in_combo, combo_mode

    me = game.player
    if me.isAlive and not game.isChatOpen:
        if game.IsKeyDown(combo_key):
            combo(game, me)
        if game.IsKeyDown(red_key):
            keyRed(game, me)
        if game.IsKeyDown(blue_key):
            keyBlue(game, me)
        if game.IsKeyDown(gold_key):
            keyGold(game, me)
