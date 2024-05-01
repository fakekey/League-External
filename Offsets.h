#pragma once
class Offsets {
public:
    Offsets();

    static int GameTime;
    static int LocalPlayer;
    static int ChampionList;
    static int MinionList;
    static int MissileList;
    static int ViewProjMatrix;

    static int ChatClient;
    static int ChatIsOpen;

    static int ObjIndex;
    static int ObjTeam;
    static int ObjPos;
    static int ObjNetworkID;
    static int ObjVisible;
    static int ObjSpawnCount;
    static int ObjTargetable;
    static int ObjHealth;
    static int ObjMaxHealth;
    static int ObjMagicPen;
    static int ObjArmorPen;
    static int ObjMagicResPen;
    static int ObjLethality;
    static int ObjBonusAttack;
    static int ObjMagicDamage;
    static int ObjBaseAttack;
    static int ObjBonusAttackSpeed;
    static int ObjArmor;
    static int ObjMagicRes;
    static int ObjMoveSpeed;
    static int ObjAttackRange;
    static int ObjName;
    static int ObjLevel;

    static int ObjSpellBook;
    static int SpellSlotSpellInfo;
    static int SpellInfoSpellData;
    static int SpellDataSpellName;
    static int SpellSlotLevel;
    static int SpellSlotTime;

    static int AIManagerAddr;
    static int AIManagerStartPath;
    static int AIManagerEndPath;
    static int AIManagerTargetPos;
    static int AIManagerSegmentCount;
    static int AIManagerMoveSpeed;
    static int AIManagerIsMoving;
    static int AIManagerDashSpeed;
    static int AIManagerIsDashing;
    static int AiManagerCurrentSegment;
    static int ServerPos;
    static int Velocity;

    static int ObjBuffManager;
    static int BuffArrayBegin;
    static int BuffArrayEnd;
    static int BuffEntryBuff;
    static int BuffType;
    static int BuffEntryBuffStartTime;
    static int BuffEntryBuffEndTime;
    static int BuffEntryBuffCount;
    static int BuffEntryBuffCountAlt;
    static int BuffEntryBuffCountAlt2;
    static int BuffName;

    static int MissileKey;
    static int MissileValue;
    static int MissileRoot;
    static int MissileCount;
    static int MissileStartPos;
    static int MissileEndPos;
    static int MissileInfo;
    static int MissileSrcIdx;

    static int ActiveSpellRoot;
    static int ActiveSpellInfo;
    static int ActiveSpellStartPos;
    static int ActiveSpellEndPos;

    static int ObjItemList;
    static int ItemInfo;
    static int ItemInfoId;
};