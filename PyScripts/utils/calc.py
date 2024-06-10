from Vippro import *
from pynput import mouse, keyboard
from pynput.mouse import Button
from time import sleep
import math

mouse_ctr = mouse.Controller()
keyboard_ctr = keyboard.Controller()

clones = {
    "shaco": [0, False, False, "shaco_square"],
    "leblanc": [0, False, False, "leblanc_square"],
    "monkeyking": [0, False, False, "monkeyking_square"],
    "neeko": [0, False, False, "neeko_square"],
    "fiddlesticks": [0, False, False, "fiddlesticks_square"],
}


def on_hit_start_item(me, target):
    if target.HasUnitTags("Unit_Minion_Lane"):
        return 5.0
    return 0.0


def on_hit_recurve_bow(me, target):
    return 15.0


def on_hit_wits_end(me, target):
    if me.level < 9:
        return 15.0
    elif me.level == 9:
        return 25.0
    elif me.level == 10:
        return 35.0
    elif me.level == 11:
        return 45.0
    elif me.level == 12:
        return 55.0
    elif me.level == 13:
        return 65.0
    elif me.level == 14:
        return 75.0
    elif me.level == 15:
        return 76.0
    elif me.level == 16:
        return 77.0
    elif me.level == 17:
        return 78.0
    elif me.level == 18:
        return 78.0


def on_hit_nashors_tooth(me, target):
    return 15.0 + 0.2 * me.abilityPower


def on_hit_guinsoos_rageblade(me, target):
    return 30.0


def on_hit_blade_of_the_ruined_king(me, target):
    dmg = target.health * 9.0 / 100.0 if not me.IsRanged else target.health * 6.0 / 100.0
    if dmg > 100.0 and not target.HasUnitTags("Unit_Champion"):
        return 100.0
    return dmg


on_hit_physical = {
    1043: on_hit_recurve_bow,
    3070: on_hit_start_item,
    1056: on_hit_start_item,
    1054: on_hit_start_item,
    3153: on_hit_blade_of_the_ruined_king,
}

on_hit_magical = {
    3091: on_hit_wits_end,
    3115: on_hit_nashors_tooth,
    3124: on_hit_guinsoos_rageblade,
    3302: on_hit_guinsoos_rageblade,
}


def get_onhit_physical(me, target):
    global on_hit_physical

    try:
        phys = 0.0
        for item in me.itemSlots:
            if item.info is None:
                continue
            if item.info.id in on_hit_physical:
                phys += on_hit_physical[item.info.id](me, target)

        return phys
    except:
        return 0.0


def get_onhit_magical(me, target):
    global on_hit_magical

    try:
        magical = 0.0
        for item in me.itemSlots:
            if item.info is None:
                continue
            if item.info.id in on_hit_magical:
                magical += on_hit_magical[item.info.id](me, target)

        return magical
    except:
        return 0.0


def dmg_hit_on_armor(me, target, dmg=0):
    calc_armor = target.armor * me.armorPen - me.lethality
    calc_armor = 0.0 if calc_armor < 0.0 else calc_armor
    return (dmg * 100.0) / (calc_armor + 100.0)


def dmg_hit_on_magic_res(me, target, dmg=0):
    calc_magic_res = target.magicRes * me.magicResPen - me.magicPen
    calc_magic_res = 0.0 if calc_magic_res < 0.0 else calc_magic_res
    return (dmg * 100.0) / (calc_magic_res + 100.0)


def is_last_hitable(me, enemy):
    hit_dmg = dmg_hit_on_armor(me, enemy, (me.basicAtk + me.bonusAtk + get_onhit_physical(me, enemy))) + dmg_hit_on_magic_res(me, enemy, get_onhit_magical(me, enemy))
    return enemy.health - hit_dmg <= 0.0


def get_lasthitable(me, minions: list, range=0.0):
    target = None
    if range == 0.0:
        range = me.attackRange + me.info.gameplayRadius
    for minion in minions:
        if minion.IsAllyTo(me):
            continue
        if not minion.IsHurtable or (minion.position.distance_squared(me.position) >= (range + minion.info.gameplayRadius / 2) ** 2):
            continue
        if is_last_hitable(me, minion):
            return minion

    return target


def get_best_target_in_range(me, champions: list, range=0.0):
    num = 999999999
    target = None
    if range == 0.0:
        range = me.attackRange + me.info.gameplayRadius
    for champ in champions:
        if champ.IsAllyTo(me):
            continue
        if champ.HasUnitTags("Unit_Champion_Clone"):
            continue
        if champ.name in clones and champ.R.name == champ.D.name:
            continue
        if not champ.IsHurtable or (champ.position.distance_squared(me.position) >= (range + champ.info.gameplayRadius / 2) ** 2):
            continue
        if num >= champ.health:
            num = champ.health
            target = champ
        if is_last_hitable(me, champ):
            return champ

    return target


def get_best_minions_in_range(me, minions: list, range=0.0):
    num = 999999999
    target = None
    if range == 0.0:
        range = me.attackRange + me.info.gameplayRadius
    for minion in minions:
        if minion.IsAllyTo(me):
            continue
        if not minion.IsHurtable or (minion.position.distance_squared(me.position) >= (range + minion.info.gameplayRadius / 2) ** 2):
            continue
        if num >= minion.health:
            num = minion.health
            target = minion
        if is_last_hitable(me, minion):
            return minion

    return target


def predict_pos(game, target, cumTime: float):
    if not target.AIManager.isMoving:
        return target.AIManager.serverPos
    if target.AIManager.isDashing:
        return target.AIManager.endPath

    if len(target.AIManager.targetPath) <= 1:
        dir = target.AIManager.endPath.sub(target.AIManager.startPath).normalize()
    else:
        dir = target.AIManager.targetPath[1] .sub(target.AIManager.targetPath[0]).normalize()

    distance_to_travel = target.AIManager.moveSpeed * (cumTime + (game.ping / 2000.0))
    return target.AIManager.serverPos.add(dir.scale(distance_to_travel))


def circle_on_line(A, B, C, R):
    # A: start of the line
    # B: end of the line
    # C: center of the circle
    # R: Radius of the circle

    # Compute the distance between A and B.
    x_diff = B.x - A.x
    y_diff = B.y - A.y
    LAB = math.sqrt(x_diff ** 2 + y_diff ** 2)

    # Compute the direction vector D from A to B.
    Dx = x_diff / LAB
    Dy = y_diff / LAB

    # The equation of the line AB is x = Dx*t + Ax, y = Dy*t + Ay with 0 <= t <= LAB.

    # Compute the distance between the points A and E, where
    # E is the point of AB closest the circle center (Cx, Cy)
    t = Dx * (C.x - A.x) + Dy * (C.y - A.y)
    if not -R <= t <= LAB + R:
        return False

    # Compute the coordinates of the point E using the equation of the line AB.
    Ex = t * Dx + A.x
    Ey = t * Dy + A.y

    # Compute the distance between E and C
    x_diff1 = Ex - C.x
    y_diff1 = Ey - C.y
    LEC = math.sqrt(x_diff1 ** 2 + y_diff1 ** 2)

    return LEC <= R


def is_collisioned(game, target, oType="minion", ability_width=0.0):
    me = game.player
    player_pos = game.WorldToScreen(me.position)
    target_pos = game.WorldToScreen(target.position)

    if oType == "minion":
        for minion in game.minions:
            if minion.IsEnemyTo(me) and minion.isAlive:
                minion_pos = game.WorldToScreen(minion.position)
                total_radius = minion.info.gameplayRadius + ability_width / 2
                if circle_on_line(player_pos, target_pos, minion_pos, total_radius):
                    return True

    if oType == "champion":
        for champ in game.champs:
            if (champ.IsEnemyTo(me) and champ.isAlive and not champ.objIndex == target.objIndex):
                champ_pos = game.WorldToScreen(champ.position)
                total_radius = champ.info.gameplayRadius + ability_width / 2
                if circle_on_line(player_pos, target_pos, champ_pos, total_radius):
                    return True

    return False


def ClickAt(left_btn: bool, pos):
    old_pos = mouse_ctr.position
    mouse_ctr.position = (pos.x, pos.y)
    mouse_ctr.press(Button.left if left_btn else Button.right)
    mouse_ctr.release(Button.left if left_btn else Button.right)
    sleep(0.005)
    mouse_ctr.position = old_pos


def PressAt(key: str, pos):
    old_pos = mouse_ctr.position
    mouse_ctr.position = (pos.x, pos.y)
    keyboard_ctr.press(key)
    keyboard_ctr.release(key)
    sleep(0.005)
    mouse_ctr.position = old_pos


def PressRightClick():
    mouse_ctr.press(Button.right)
    mouse_ctr.release(Button.right)


def PressLeftClick():
    mouse_ctr.press(Button.left)
    mouse_ctr.release(Button.left)


def PressKey(key: str):
    keyboard_ctr.press(key)
    keyboard_ctr.release(key)


def MoveAndTrigger(key: str, pos):
    old_pos = mouse_ctr.position
    mouse_ctr.position = (pos.x, pos.y)
    PressLeftClick()
    PressKey(key)
    sleep(0.005)
    mouse_ctr.position = old_pos


def KeyDown(key: str):
    keyboard_ctr.press(key)


def KeyUp(key: str):
    keyboard_ctr.release(key)


def Trigger(key: str, charge: bool):
    if (charge):
        KeyDown(key)
    else:
        PressKey(key)


def MoveAndRelease(key: str, pos):
    old_pos = mouse_ctr.position
    mouse_ctr.position = (pos.x, pos.y)
    KeyUp(key)
    sleep(0.005)
    mouse_ctr.position = old_pos


def MoveWhilePress(key: str, fromPos, toPos):
    old_pos = mouse_ctr.position
    try:
        mouse_ctr.position = (fromPos.x, fromPos.y)
        PressKey(key)
        sleep(0.005)
        mouse_ctr.position = (toPos.x, toPos.y)
        sleep(0.005)
        mouse_ctr.position = old_pos
    except Exception as ex:
        print(ex)


def Cast(key: str, fromPos, toPos):
    MoveWhilePress(key, fromPos, toPos)


def GetCursor():
    return Vector2(mouse_ctr.position[0], mouse_ctr.position[1])


def MoveCursor(pos):
    mouse_ctr.position = (pos.x, pos.y)
