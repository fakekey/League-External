from Vippro import *

Vippro_script_info = {
    "script": "Draw",
    "author": "nghilq",
    "description": "Draw some fancy shits",
    "target_champ": "draw",
}

enemy_ranges = False
attack_range = False
my_missile = False
enemy_missile = False
my_path = False
enemy_path = False

colors = {
    0: Colors.YELLOW,
    1: Colors.GREEN,
    2: Colors.PURPLE,
    3: Colors.WHITE,
    4: Colors.CYAN,
    5: Colors.RED,
    6: Colors.ORANGE,
    7: Colors.BLUE,
}
selectedColor = 0


def Vippro_load_cfg(cfg):
    global selectedColor
    global enemy_ranges, attack_range
    global my_missile, enemy_missile
    global my_path, enemy_path
    enemy_ranges = cfg.GetBool("enemy_ranges", True)
    attack_range = cfg.GetBool("attack_range", True)
    my_missile = cfg.GetBool("my_missile", True)
    enemy_missile = cfg.GetBool("enemy_missile", True)
    my_path = cfg.GetBool("my_path", True)
    enemy_path = cfg.GetBool("enemy_path", True)
    selectedColor = cfg.GetInt("selectedColor", 0)


def Vippro_save_cfg(cfg):
    global selectedColor
    global enemy_ranges, attack_range
    global my_missile, enemy_missile
    global my_path, enemy_path
    cfg.SetBool("enemy_ranges", enemy_ranges)
    cfg.SetBool("attack_range", attack_range)
    cfg.SetBool("my_missile", my_missile)
    cfg.SetBool("enemy_missile", enemy_missile)
    cfg.SetBool("my_path", my_path)
    cfg.SetBool("enemy_path", enemy_path)
    cfg.SetInt("selectedColor", selectedColor)


def Vippro_draw_settings(game, ui):
    global enemy_ranges, attack_range
    global my_missile, enemy_missile
    global selectedColor, colors
    global my_path, enemy_path

    selectedColor = ui.ListBox(
        "##",
        ["YELLOW", "GREEN", "PURPLE", "WHITE", "CYAN", "RED", "ORANGE", "BLUE"],
        selectedColor,
    )

    enemy_ranges = ui.Checkbox("Draw enemy ranges", enemy_ranges)
    attack_range = ui.Checkbox("Champion attack range", attack_range)
    enemy_missile = ui.Checkbox("Draw enemy missiles", enemy_missile)
    my_missile = ui.Checkbox("Champion missile", my_missile)
    enemy_path = ui.Checkbox("Draw enemy pathes", enemy_path)
    my_path = ui.Checkbox("Champion pathes", my_path)


def draw_path(game, player):
    global selectedColor, colors
    if (
        player.isAlive
        and player.visible
        and game.IsScreenPointOnScreen(player.position)
    ):
        if len(player.AIManager.targetPath) <= 1:
            pathes = [player.AIManager.startPath, player.AIManager.endPath]
        else:
            pathes = player.AIManager.targetPath
        for i in range(len(pathes) - 1):
            game.DrawCircleWorldFilled(
                pathes[i],
                12,
                colors[selectedColor],
            )
            game.DrawLineWorld(
                pathes[i],
                pathes[i + 1],
                1,
                colors[selectedColor],
            )
            game.DrawCircleWorldFilled(
                pathes[i + 1],
                12,
                colors[selectedColor],
            )


def draw_enemy_path(game, player, champ):
    global selectedColor, colors
    if champ.IsAllyTo(player):
        return
    if champ.isAlive and champ.visible and game.IsScreenPointOnScreen(champ.position):
        if len(champ.AIManager.targetPath) <= 1:
            pathes = [champ.AIManager.startPath, champ.AIManager.endPath]
        else:
            pathes = champ.AIManager.targetPath
        for i in range(len(pathes) - 1):
            game.DrawCircleWorldFilled(
                pathes[i],
                12,
                colors[selectedColor],
            )
            game.DrawLineWorld(
                pathes[i],
                pathes[i + 1],
                1,
                colors[selectedColor],
            )
            game.DrawCircleWorldFilled(
                pathes[i + 1],
                12,
                colors[selectedColor],
            )


def draw_atk_range(game, player):
    global selectedColor, colors
    if (
        player.isAlive
        and player.visible
        and game.IsScreenPointOnScreen(player.position)
    ):
        game.DrawCircleWorld(
            player.position,
            player.attackRange + player.info.gameplayRadius,
            1,
            colors[selectedColor],
        )


def draw_champ_ranges(game, player, champ):
    global selectedColor, colors
    if (
        champ.isAlive
        and champ.visible
        and champ.IsEnemyTo(player)
        and game.IsScreenPointOnScreen(champ.position)
    ):
        game.DrawCircleWorld(
            champ.position,
            champ.attackRange + champ.info.gameplayRadius,
            1,
            colors[selectedColor],
        )


def draw_active_cast(game, player):
    global selectedColor, colors
    if player.activeCast.info is None:
        return
    mType = player.activeCast.info.type
    # Line
    if mType == "Direction" or (
        (mType == "Location" or mType == "LocationClamped")
        and player.activeCast.info.width != 0
    ):
        game.DrawRectFromPos(
            player.activeCast.startPos,
            player.activeCast.endPos,
            player.activeCast.info.width,
            1,
            colors[selectedColor],
        )
        game.DrawRectFromPos(
            player.activeCast.startPos,
            player.activeCast.endPos,
            player.info.gameplayRadius * 2,
            1,
            colors[selectedColor],
        )
    # Circle
    if (
        mType == "Area" or mType == "AreaClamped"
    ) and player.activeCast.info.castRadius != 0:
        game.DrawCircleWorld(
            player.activeCast.endPos,
            player.activeCast.info.castRadius,
            1,
            colors[selectedColor],
        )


def draw_missile(game, player, missile):
    global selectedColor, colors
    if missile.srcIdx != player.objIndex:
        return
    mType = missile.info.type
    # Line
    if mType == "Direction" or (
        (mType == "Location" or mType == "LocationClamped") and missile.info.width != 0
    ):
        game.DrawRectFromPos(
            missile.pos,
            missile.endPos,
            missile.info.width,
            1,
            colors[selectedColor],
        )
        game.DrawRectFromPos(
            missile.pos,
            missile.endPos,
            player.info.gameplayRadius * 2,
            1,
            colors[selectedColor],
        )
    # Circle
    if (mType == "Area" or mType == "AreaClamped") and missile.info.castRadius != 0:
        game.DrawCircleWorld(
            missile.pos,
            missile.info.castRadius,
            1,
            colors[selectedColor],
        )
        game.DrawCircleWorld(
            missile.endPos,
            missile.info.castRadius,
            1,
            colors[selectedColor],
        )


def draw_enemy_active_cast(game, player, champ):
    global selectedColor, colors
    if champ.IsAllyTo(player):
        return
    if champ.activeCast.info is None:
        return
    mType = champ.activeCast.info.type
    # Line
    if mType == "Direction" or (
        (mType == "Location" or mType == "LocationClamped")
        and champ.activeCast.info.width != 0
    ):
        game.DrawRectFromPos(
            champ.activeCast.startPos,
            champ.activeCast.endPos,
            champ.activeCast.info.width,
            1,
            colors[selectedColor],
        )
        game.DrawRectFromPos(
            champ.activeCast.startPos,
            champ.activeCast.endPos,
            champ.info.gameplayRadius * 2,
            1,
            colors[selectedColor],
        )
    # Circle
    if (
        mType == "Area" or mType == "AreaClamped"
    ) and champ.activeCast.info.castRadius != 0:
        game.DrawCircleWorld(
            champ.activeCast.endPos,
            champ.activeCast.info.castRadius,
            1,
            colors[selectedColor],
        )


def draw_enemy_missile(game, player, missile):
    global selectedColor, colors
    if missile.team == player.team:
        return
    mType = missile.info.type
    owner = game.GetObjectByIndex(missile.srcIdx)
    if not owner:
        return
    # Line
    if mType == "Direction" or (
        (mType == "Location" or mType == "LocationClamped") and missile.info.width != 0
    ):
        game.DrawRectFromPos(
            missile.pos,
            missile.endPos,
            missile.info.width,
            1,
            colors[selectedColor],
        )
        game.DrawRectFromPos(
            missile.pos,
            missile.endPos,
            owner.info.gameplayRadius * 2,
            1,
            colors[selectedColor],
        )
    # Circle
    if (mType == "Area" or mType == "AreaClamped") and missile.info.castRadius != 0:
        game.DrawCircleWorld(
            missile.pos,
            missile.info.castRadius,
            1,
            colors[selectedColor],
        )
        game.DrawCircleWorld(
            missile.endPos,
            missile.info.castRadius,
            1,
            colors[selectedColor],
        )


def Vippro_update(game, ui):
    global attack_range, enemy_ranges
    global my_missile, enemy_missile
    global my_path, enemy_path

    if game.player.isAlive and not game.isChatOpen:
        if attack_range:
            draw_atk_range(game, game.player)
        if my_missile:
            draw_active_cast(game, game.player)
        if my_path:
            draw_path(game, game.player)

        for champ in game.champs:
            if enemy_ranges:
                draw_champ_ranges(game, game.player, champ)
            if enemy_missile:
                draw_enemy_active_cast(game, game.player, champ)
            if enemy_path:
                draw_enemy_path(game, game.player, champ)

        for missile in game.missiles:
            if my_missile:
                draw_missile(game, game.player, missile)
            if enemy_missile:
                draw_enemy_missile(game, game.player, missile)
