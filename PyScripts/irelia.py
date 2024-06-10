from Vippro import *
from utils.calc import *
from time import sleep

Vippro_script_info = {
    "script": "Irelia",
    "author": "nghilq",
    "description": "Irelking simulator",
    "target_champ": "irelia",
}

combo_key = 57
laneclear_key = 47
lasthit_key = 45
use_q_in_combo = True
use_e_in_combo = True
lane_clear_with_q = True
lasthit_with_q = True


def Vippro_load_cfg(cfg):
    global combo_key, laneclear_key, lasthit_key
    global use_q_in_combo, use_e_in_combo
    global lane_clear_with_q, lasthit_with_q

    combo_key = cfg.GetInt("combo_key", 57)
    laneclear_key = cfg.GetInt("laneclear_key", 47)
    lasthit_key = cfg.GetInt("lasthit_key", 45)
    use_q_in_combo = cfg.GetBool("use_q_in_combo", True)
    use_e_in_combo = cfg.GetBool("use_e_in_combo", True)
    lane_clear_with_q = cfg.GetBool("lane_clear_with_q", True)
    lasthit_with_q = cfg.GetBool("lasthit_with_q", True)


def Vippro_save_cfg(cfg):
    global combo_key, laneclear_key, lasthit_key
    global use_q_in_combo, use_e_in_combo
    global lane_clear_with_q, lasthit_with_q

    cfg.SetInt("combo_key", combo_key)
    cfg.SetInt("laneclear_key", laneclear_key)
    cfg.SetInt("lasthit_key", lasthit_key)
    cfg.SetBool("use_q_in_combo", use_q_in_combo)
    cfg.SetBool("use_e_in_combo", use_e_in_combo)
    cfg.SetBool("lane_clear_with_q", lane_clear_with_q)
    cfg.SetBool("lasthit_with_q", lasthit_with_q)


def Vippro_draw_settings(game, ui):
    global combo_key, laneclear_key, lasthit_key
    global use_q_in_combo, use_e_in_combo
    global lane_clear_with_q, lasthit_with_q

    combo_key = ui.KeySelect("Combo key", combo_key)
    laneclear_key = ui.KeySelect("Laneclear key", laneclear_key)
    lasthit_key = ui.KeySelect("Lasthit key", lasthit_key)

    if ui.TreeNode("Setting [Q]"):
        use_q_in_combo = ui.Checkbox("Use Q in Combo", use_q_in_combo)
        lane_clear_with_q = ui.Checkbox("Laneclear with Q", lane_clear_with_q)
        lasthit_with_q = ui.Checkbox("Lasthit with Q", lasthit_with_q)
        ui.TreePop()
    ui.Separator()

    if ui.TreeNode("Setting [E]"):
        use_e_in_combo = ui.Checkbox("Use E in Combo", use_e_in_combo)
        ui.TreePop()
    ui.Separator()


qLvLDmg = [5, 25, 45, 65, 85]
qMinionDmg = 0.0
passiveDmg = 0.0
lastQ: dict[int, float] = {}


def qDmg(game, target):
    global qLvLDmg, qMinionDmg, passiveDmg

    me = game.player
    playerLvl = me.level
    totalatk = me.basicAtk + me.bonusAtk
    qMinionDmg = 43 + (playerLvl * 12)

    buff = me.GetBuffByName("ireliapassivestacksmax")
    if buff and buff.isAlive:
        passiveDmg = (7 + (playerLvl * 3)) + (me.bonusAtk * 0.2)
    else:
        passiveDmg = 0

    return qLvLDmg[me.Q.level - 1] + (totalatk * 0.6) + get_onhit_physical(me, target), passiveDmg + get_onhit_magical(me, target)


def CanQ(game, target) -> bool:
    global qMinionDmg, lastQ

    me = game.player
    totalDmg = 0.0
    dmgQad, dmgQap = qDmg(game, target)
    if target.HasUnitTags("Unit_Minion_Lane"):
        totalDmg = dmg_hit_on_armor(me, target, dmgQad + qMinionDmg) + dmg_hit_on_magic_res(me, target, dmgQap)
    else:
        totalDmg = dmg_hit_on_armor(me, target, dmgQad) + dmg_hit_on_magic_res(me, target, dmgQap)

    if target.health - totalDmg < 0.0:
        return True

    if target.HasUnitTags("Unit_Champion"):
        buff = target.GetBuffByName("ireliamark")
        if buff and buff.countAlt > 0:
            lastTimeQ = lastQ.get(target.objIndex, None)
            if lastTimeQ is None:
                return True
            else:
                if game.gameTime - lastTimeQ >= 0.5:
                    return True

    return False


def TargetSelectorWeakest(game, range=0.0):
    num = 9999999
    target = None
    me = game.player
    for champ in game.champs:
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
        if is_last_hitable_with_Q(game, champ):
            return champ

    return target


def get_closest_mob_to_enemy_for_gap(game, enemy):
    me = game.player
    for minion in game.minions:
        if minion.IsAllyTo(me):
            continue
        if not minion.IsHurtable or (
            minion.position.distance(me.position)
            >= 600 + (minion.info.gameplayRadius / 2)
        ):
            continue
        d_me_enemy = me.position.distance_squared(enemy.position)
        d_enemy_minion = enemy.position.distance_squared(minion.position)
        if d_enemy_minion < d_me_enemy and is_last_hitable_with_Q(game, minion):
            return minion


def is_last_hitable_with_Q(game, enemy):
    if CanQ(game, enemy):
        return True

    return False


def TargetSelector(game, enemies, range=0.0):
    target = None
    me = game.player
    for champ in enemies:
        if champ.IsAllyTo(me):
            continue
        if champ.HasUnitTags("Unit_Champion_Clone"):
            continue
        if champ.name in clones and champ.R.name == champ.D.name:
            continue
        if not champ.IsHurtable or (champ.position.distance_squared(me.position) >= (range + champ.info.gameplayRadius / 2) ** 2):
            continue
        if is_last_hitable_with_Q(game, champ):
            return champ

    return target


def castE1(game, pos):
    me = game.player
    if me.E.name == "ireliae":
        me.E.MoveAndTrigger(game.WorldToScreen(pos))


def castE2(game, target):
    me = game.player
    if me.E.name == "ireliae2":
        predict = predict_pos(game, target, 0.1)
        me.E.MoveAndTrigger(game.WorldToScreen(predict.add((predict.sub(me.position)).normalize().scale(750))))


def combo(game):
    global lastQ
    global use_q_in_combo, use_e_in_combo

    me = game.player
    if me.isAlive and me.Q.IsReady(game.gameTime) and use_q_in_combo:
        target = TargetSelector(game, game.champs, 600)
        if target:
            me.Q.MoveAndTrigger(game.WorldToScreen(target.position))
            lastQ[target.objIndex] = game.gameTime
            sleep(0.02)
            return

    if me.isAlive and me.Q.IsReady(game.gameTime) and use_q_in_combo:
        target2 = TargetSelectorWeakest(game, 3000.0)
        if target2:
            if (me.E.IsReady(game.gameTime) and use_e_in_combo and me.position.distance_squared(target2.position) <= 750 ** 2):
                castE2(game, target2)
            minion = get_closest_mob_to_enemy_for_gap(game, target2)
            if minion:
                me.Q.MoveAndTrigger(game.WorldToScreen(minion.position))
                if (me.E.IsReady(game.gameTime) and use_e_in_combo and minion.position.distance_squared(target2.position) <= 750 ** 2):
                    sleep(0.1)
                    castE1(game, minion.position)
                    return


def laneclear(game, mode=0):
    global lane_clear_with_q, lasthit_with_q

    me = game.player
    if me.isAlive and me.Q.IsReady(game.gameTime):
        if (mode == 0 and lane_clear_with_q) or (mode == 1 and lasthit_with_q):
            target = TargetSelector(game, [*game.minions, *game.jungles], 600)
            if target:
                me.Q.MoveAndTrigger(game.WorldToScreen(target.position))
                sleep(0.02)


def Vippro_update(game, ui):
    global combo_key, laneclear_key, lasthit_key
    global use_q_in_combo, use_e_in_combo
    global lane_clear_with_q, lasthit_with_q

    me = game.player
    if me.isAlive and not game.isChatOpen:
        if game.IsKeyDown(combo_key):
            combo(game)
        if game.IsKeyDown(laneclear_key):
            laneclear(game, 0)
        if game.IsKeyDown(lasthit_key):
            laneclear(game, 1)
