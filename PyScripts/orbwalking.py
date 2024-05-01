from Vippro import *
from utils.calc import *
from time import sleep

Vippro_script_info = {
    "script": "Orbwalk",
    "author": "nghilq",
    "description": "Simple orbwalk",
}

key_orbwalk = 57
laneclear_key = 47
lasthit_key = 45
champ_only_key = 42
press_champ_only_key = False

click_speed = 0
kite_ping = 0


def Vippro_load_cfg(cfg):
    global key_orbwalk, laneclear_key, lasthit_key
    global click_speed, kite_ping
    global champ_only_key, press_champ_only_key

    key_orbwalk = cfg.GetInt("key_orbwalk", 57)
    laneclear_key = cfg.GetInt("laneclear_key", 47)
    lasthit_key = cfg.GetInt("lasthit_key", 45)
    click_speed = cfg.GetInt("click_speed", 30)
    kite_ping = cfg.GetInt("kite_ping", 24)
    champ_only_key = cfg.GetInt("champ_only_key", 42)
    press_champ_only_key = cfg.GetBool("press_champ_only_key", False)


def Vippro_save_cfg(cfg):
    global key_orbwalk, laneclear_key, lasthit_key
    global click_speed, kite_ping
    global champ_only_key, press_champ_only_key

    cfg.SetInt("laneclear_key", laneclear_key)
    cfg.SetInt("key_orbwalk", key_orbwalk)
    cfg.SetInt("lasthit_key", lasthit_key)
    cfg.SetFloat("click_speed", click_speed)
    cfg.SetFloat("kite_ping", kite_ping)
    cfg.SetInt("champ_only_key", champ_only_key)
    cfg.SetBool("press_champ_only_key", press_champ_only_key)


def Vippro_draw_settings(game, ui):
    global key_orbwalk, laneclear_key, lasthit_key
    global click_speed, kite_ping
    global champ_only_key, press_champ_only_key

    if ui.TreeNode("Key settings"):
        laneclear_key = ui.KeySelect("Laneclear key", laneclear_key)
        lasthit_key = ui.KeySelect("Lasthit key", lasthit_key)
        key_orbwalk = ui.KeySelect("Orbwalk key", key_orbwalk)
        champ_only_key = ui.KeySelect("Champion only key", champ_only_key)
        ui.TreePop()
    press_champ_only_key = ui.Checkbox("Press champion only key", press_champ_only_key)
    click_speed = ui.SliderInt("Click speed", int(click_speed), 30, 100)
    kite_ping = ui.SliderInt("Game ping", int(kite_ping), 0, 100)


attackTimer = 0.0
moveTimer = 0.0
humanizer = 0.0
c_atk_speed = 0


def Vippro_update(game, ui):
    global key_orbwalk, laneclear_key, lasthit_key
    global click_speed, kite_ping
    global attackTimer, moveTimer, humanizer
    global c_atk_speed
    global champ_only_key, press_champ_only_key

    me = game.player
    if me.isAlive and not game.isChatOpen:
        c_atk_speed = me.GetAttackSpeed()
        atk_delay = 1.0 / c_atk_speed
        cast_delay = me.GetCastDelay(c_atk_speed)

        if game.IsKeyDown(key_orbwalk):
            if press_champ_only_key:
                game.KeyDown(champ_only_key)

            target = get_best_target_in_range(me, game.champs)

            if game.gameTime > attackTimer and target:
                pos = game.WorldToScreen(target.position)
                old_pos = game.GetCursor()
                game.MoveCursor(pos)
                game.PressRightClick()
                attackTimer = game.gameTime + atk_delay
                moveTimer = game.gameTime + cast_delay + (kite_ping / 2000.0)
                sleep(kite_ping / 2000.0)
                game.MoveCursor(old_pos)
            else:
                if game.gameTime > humanizer:
                    if game.gameTime > moveTimer:
                        game.PressRightClick()
                    humanizer = game.gameTime + (click_speed / 1000.0)

        elif game.IsKeyDown(laneclear_key):
            if press_champ_only_key:
                game.KeyDown(champ_only_key)
            # Lock camera
            game.KeyDown(59)

            target = get_best_minions_in_range(me, [*game.minions, *game.jungles])

            if game.gameTime > attackTimer and target:
                pos = game.WorldToScreen(target.position)
                old_pos = game.GetCursor()
                game.MoveCursor(pos)
                if press_champ_only_key:
                    game.KeyUp(champ_only_key)
                game.PressRightClick()
                attackTimer = game.gameTime + atk_delay
                moveTimer = game.gameTime + cast_delay + (kite_ping / 2000.0)
                sleep(kite_ping / 2000.0)
                game.MoveCursor(old_pos)
            else:
                if game.gameTime > humanizer:
                    if game.gameTime > moveTimer:
                        game.PressRightClick()
                    humanizer = game.gameTime + (click_speed / 1000.0)

        elif game.IsKeyDown(lasthit_key):
            if press_champ_only_key:
                game.KeyDown(champ_only_key)
            # Lock camera
            game.KeyDown(59)

            target = get_lasthitable(me, [*game.minions, *game.jungles])

            if game.gameTime > attackTimer and target:
                pos = game.WorldToScreen(target.position)
                old_pos = game.GetCursor()
                game.MoveCursor(pos)
                if press_champ_only_key:
                    game.KeyUp(champ_only_key)
                game.PressRightClick()
                attackTimer = game.gameTime + atk_delay
                moveTimer = game.gameTime + cast_delay + (kite_ping / 2000.0)
                sleep(kite_ping / 2000.0)
                game.MoveCursor(old_pos)
            else:
                if game.gameTime > humanizer:
                    if game.gameTime > moveTimer:
                        game.PressRightClick()
                    humanizer = game.gameTime + (click_speed / 1000.0)

        else:
            # Unlock camera
            game.KeyUp(59)

            if press_champ_only_key:
                game.KeyUp(champ_only_key)
