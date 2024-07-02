#include "Offsets.h"

Offsets::Offsets() {};

int Offsets::GameTime = 0x193A5C8; // f3 0f 11 3d ? ? ? ? 48 8b 49
int Offsets::LocalPlayer = 0x194E988; // 48 8b 0d ? ? ? ? 48 85 c9 74 ? 48 81 c1 ? ? ? ? 48 89 5c 24 ? 48 89 7c 24
int Offsets::ChampionList = 0x192D8C8; // 48 8b 05 ? ? ? ? f3 0f 10 35 ? ? ? ? 48 8b 70
int Offsets::MinionList = 0x1930668; // 48 8b 05 ? ? ? ? f3 0f 10 40 ? 48 83 c4
int Offsets::MissileList = 0x1930680; // 4C 8B 05 ? ? ? ? 49 8B 58 08
int Offsets::ViewProjMatrix = 0x19A4310; // 48 8d 0d ? ? ? ? c7 45 ? ? ? ? ? e8 ? ? ? ? 48 8d 55

int Offsets::ChatClient = 0x194DCF8; // 48 8b 0d ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 84 c0 74 ? 48 8b 0d ? ? ? ? e8 ? ? ? ? 48 8b 0d ? ? ? ? 48 85 c9 74 ? e8 ? ? ? ? 48 8b 0d ? ? ? ? e8
int Offsets::ChatIsOpen = 0x5A0;

int Offsets::ObjIndex = 0x10;
int Offsets::ObjTeam = 0x3C;
int Offsets::ObjPos = 0x220;
int Offsets::ObjNetworkID = 0xC8;
int Offsets::ObjVisible = 0x330;
int Offsets::ObjSpawnCount = 0x360;
int Offsets::ObjTargetable = 0x1018;
int Offsets::ObjHealth = 0x11E8;
int Offsets::ObjMaxHealth = 0x1210;
int Offsets::ObjMagicPen = 0x1834;
int Offsets::ObjArmorPen = 0x1838;
int Offsets::ObjMagicResPen = 0x183C;
int Offsets::ObjLethality = 0x1850;
int Offsets::ObjBonusAttack = 0x1890;
int Offsets::ObjMagicDamage = 0x18A0;
int Offsets::ObjBaseAttack = 0x1938;
int Offsets::ObjBonusAttackSpeed = 0x1904;
int Offsets::ObjArmor = 0x195C;
int Offsets::ObjMagicRes = 0x1964;
int Offsets::ObjMoveSpeed = 0x1974;
int Offsets::ObjAttackRange = 0x197C;
int Offsets::ObjName = 0x4560;
int Offsets::ObjLevel = 0x4FA8;

int Offsets::ObjSpellBook = 0x3DE8;
int Offsets::SpellSlotSpellInfo = 0x140;
int Offsets::SpellInfoSpellData = 0x50;
int Offsets::SpellDataSpellName = 0x80;
int Offsets::SpellSlotLevel = 0x28;
int Offsets::SpellSlotTime = 0x30;

int Offsets::AIManagerAddr = 0x4418;
int Offsets::AIManagerStartPath = 0x2F0;
int Offsets::AIManagerEndPath = 0x2FC;
int Offsets::AIManagerTargetPos = 0x308;
int Offsets::AIManagerSegmentCount = 0x310;
int Offsets::AIManagerMoveSpeed = 0x2D8;
int Offsets::AIManagerIsMoving = 0x2DC;
int Offsets::AIManagerDashSpeed = 0x320;
int Offsets::AIManagerIsDashing = 0x344;
int Offsets::AiManagerCurrentSegment = 0x2E0;
int Offsets::ServerPos = 0x54C;
int Offsets::Velocity = 0x440;

int Offsets::ObjBuffManager = 0x3080;
int Offsets::BuffArrayBegin = 0x18;
int Offsets::BuffArrayEnd = 0x20;
int Offsets::BuffEntryBuff = 0x10;
int Offsets::BuffType = 0xC;
int Offsets::BuffEntryBuffStartTime = 0x18;
int Offsets::BuffEntryBuffEndTime = 0x1C;
int Offsets::BuffEntryBuffCount = 0x8C;
int Offsets::BuffEntryBuffCountAlt = 0x38;
int Offsets::BuffEntryBuffCountAlt2 = 0x3C;
int Offsets::BuffName = 0x8;

int Offsets::MissileKey = 0x20;
int Offsets::MissileValue = 0x28;
int Offsets::MissileRoot = 0x8;
int Offsets::MissileCount = 0x10;
int Offsets::MissileStartPos = 0x3B0;
int Offsets::MissileEndPos = 0x3BC;
int Offsets::MissileInfo = 0x2E8;
int Offsets::MissileSrcIdx = 0x380;

int Offsets::ActiveSpellRoot = 0x3320;
int Offsets::ActiveSpellInfo = 0x8;
int Offsets::ActiveSpellStartPos = 0xD0;
int Offsets::ActiveSpellEndPos = 0xDC;

int Offsets::ObjItemList = 0x5040;
int Offsets::ItemInfo = 0x30;
int Offsets::ItemInfoId = 0x9c;
