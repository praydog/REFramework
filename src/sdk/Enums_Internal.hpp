// Generated with REFramework
#pragma once

#include <cstdint>

namespace app::ropeway::WwiseOptionMenu {
    enum class Speaker : int64_t {
        TV = 0,
        Headphone = 1,
        Surround = 2,
        Atmos = 3,
    };
}
namespace app::ropeway::rogue::RogueDefine {
    enum class ItemDropCategory : int64_t {
        None = 0xFFFFFFFF,
        RandomSet = 1000,
        Pattern_00 = 0,
        Pattern_01 = 1,
        Pattern_02 = 2,
        Pattern_03 = 3,
        Pattern_04 = 4,
        Pattern_05 = 5,
        Pattern_06 = 6,
        Pattern_07 = 7,
        Pattern_08 = 8,
        Pattern_09 = 9,
        Pattern_10 = 10,
        Pattern_11 = 11,
        Pattern_12 = 12,
        Pattern_13 = 13,
        Pattern_14 = 14,
        Pattern_15 = 15,
        Pattern_16 = 16,
        Pattern_17 = 17,
        Pattern_18 = 18,
        Pattern_19 = 19,
    };
}
namespace app::ropeway::motion::MotionEventHandler {
    enum class LayerKind : int64_t {
        Fast = 0,
        Default = 1,
        Late = 2,
    };
}
namespace via::hid::VrTracker {
    enum class StopStatus : int64_t {
        Success = 0,
        NotSupported = 0xFFFFFFFF,
        NotStarted = -2,
        InvalidDeviceHandle = -3,
        UnregisterDeviceFailed = -4,
    };
}
namespace app::ropeway::enemy::em6200::MotionPattern {
    enum class DirFLRBLR : int64_t {
        FrontLeft = 0,
        BackLeft = 1,
        FrontRight = 2,
        BackRight = 3,
    };
}
namespace via::dynamics {
    enum class ShapeCastOption : int64_t {
        AllHits = 0,
        DisableBackFacingTriangleHits = 1,
        DisableFrontFacingTriangleHits = 2,
        NearSort = 3,
        EnableInsideHits = 4,
        OneHitBreak = 5,
        Max = 6,
    };
}
namespace via {
    enum class ScreenShotOverlayImageOrigin : int64_t {
        LeftTop = 1,
        LeftCenter = 2,
        LeftBottom = 3,
        CenterTop = 4,
        CenterCenter = 5,
        CenterBottom = 6,
        RightTop = 7,
        RightCenter = 8,
        RightBottom = 9,
    };
}
namespace via::clr {
    enum class EnumI1 : int64_t {
        Dummy = 0,
    };
}
namespace via::render {
    enum class ClearFlag : int64_t {
        Depth = 1,
        Stencil = 2,
    };
}
namespace app::ropeway::gamemastering::TitleFlow {
    enum class State : int64_t {
        INITIALIZE_FIRST_BOOT = 0,
        INITIALIZE = 1,
        TITLE_FORCE_SWITCH_SDR = 2,
        TITLE_GUI_WAIT_ACTIVE = 3,
        TITLE_MAIN_INITIALIZE = 4,
        TITLE_PRESS_START = 5,
        TITLE_DLC_CHECK = 6,
        TITLE_DLC_DIALOGUE = 7,
        TITLE_RECORD_DIALOGUE = 8,
        TITLE_MENU_MAIN_ = 9,
        TITLE_MENU_STORY = 10,
        TITLE_EXTRA = 11,
        TITLE_BONUS = 12,
        TITLE_OPTIONS = 13,
        TITLE_COSTUME = 14,
        TITLE_MAIN_SCENARIO_LOAD = 15,
        TITLE_MAIN_SCENARIO_CONTINUE = 16,
        TITLE_MAIN_SCENARIO_NEW_GAME_LIST = 17,
        TITLE_MAIN_SCENARIO_NEW_GAME_LIST_2 = 18,
        TITLE_MAIN_SCENARIO_NEW_GAME_START = 19,
        TITLE_MAIN_SCENARIO_SELECT_DIFFICULTY = 20,
        TITLE_MAIN_SCENARIO_NEW_GAME_END = 21,
        TITLE_STORE_OPEN = 22,
        TITLE_STORE_OPEN_WAIT = 23,
        TITLE_MAIN_SCENARIO_SKIP_CAMERA = 24,
        TITLE_MAIN_SCENARIO_SKIP_CAMERA_BACK = 25,
        TITLE_SAVEDATA_LOAD_END = 26,
        TITLE_NOTICE_INSTALLING_DATA = 27,
        FINALIZE = 28,
        TITLE_DLC_CHECKWAIT = 29,
        WAIT_GUI_ANIMATION = 100,
        WAIT_TIMER = 101,
    };
}
namespace via {
    enum class GroupPlacement : int64_t {
        Top = 0,
        Bottom = 1,
        Natural = 2,
    };
}
namespace via::effect::script::EffectControlAction {
    enum class TargetModeEnum : int64_t {
        TargetOnly = 0,
        TargetAndChildren = 1,
        ChildrenOnly = 2,
    };
}
namespace app::ropeway::MirroredColliderUserData {
    enum class MirrorActionType : int64_t {
        ForwardOn = 0,
        InverseOn = 1,
    };
}
namespace app::ropeway::gui::RogueKeyFlagsBehavior {
    enum class KeyType : int64_t {
        INVALID = 0xFFFFFFFF,
        UNICORN = 0,
        VIRGIN_L = 1,
        VIRGIN_R = 2,
        LION = 3,
        SPADE = 4,
        DIAMOND = 5,
        CLOVER = 6,
        HEART = 7,
    };
}
namespace app::ropeway::ActorDefine {
    enum class ActorType : int64_t {
        INVALID = 0xFFFFFFFF,
        PL0000 = 0,
        PL1000 = 1000,
        PL2000 = 2000,
        PL3000 = 3000,
        PL4000 = 4000,
        PL4100 = 4100,
        PL5000 = 5000,
        PL5100 = 5100,
        PL5200 = 5200,
        PL5300 = 5300,
        PL5700 = 5700,
        PL6000 = 6000,
        PL6100 = 6100,
        PL6200 = 6200,
        PL7000 = 7000,
        PL7100 = 7100,
        PL7300 = 7300,
        PL7400 = 7400,
        PL7500 = 7500,
        PL7600 = 7600,
        PL7700 = 7700,
        PL7800 = 7800,
        PL8000 = 8000,
        PL8100 = 8100,
        PL8300 = 8300,
        PL8400 = 8400,
    };
}
namespace app::ropeway::gamemastering::MainFlowManager {
    enum class LostType : int64_t {
        A = 0,
        B = 1,
        C = 2,
        D = 3,
        MAX = 4,
    };
}
namespace app::ropeway::gui::BgObjectInfo {
    enum class Type : int64_t {
        Invalid = 0,
        Barricade = 1,
        Window = 2,
    };
}
namespace via::eq::Var {
    enum class Identifier : int64_t {
        None = 0,
        Constant = 1,
        Override = 2,
    };
}
namespace app::ropeway::survivor::player::PlayerForbidAimController::ForbidCheckAimWork {
    enum class AsyncCastKind : int64_t {
        Vision = 0,
        Aim = 1,
    };
}
namespace via {
    enum class SystemServiceNativeUiOverlaidStatus : int64_t {
        Unknown = 0,
        NoOverlaid = 1,
        Overlaid = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class WalkDetailKindID : int64_t {
        TYPE_A = 0,
        TYPE_B = 1,
        TYPE_C = 2,
        TYPE_D = 3,
        TYPE_E = 4,
        TYPE_F = 5,
        LIMPING_L = 6,
        LIMPING_R = 7,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class RailingFall : int64_t {
        RAILING_FALL_FENCE = 0,
        RAILING_FALL_CEILING = 1,
    };
}
namespace app::ropeway::CheckFlag {
    enum class CheckLogic : int64_t {
        AND = 0,
        OR = 1,
        ALL_NOT = 2,
        NOT_CHECK_FLAG = 3,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class Em0000PartsEnableDictKey : int64_t {
        ALWAYS = 0,
        DROP_ACCESSORY_HAT = 1,
        DROP_BODY_HEAD = 2,
        DROP_BODY_INTESTINE = 3,
        DROP_BODY_LOWERBODY = 4,
        DROP_L_ARM_DEFAULT = 5,
        DROP_L_ARM_HIGH = 6,
        DROP_L_ARM_LOW = 7,
        DROP_L_ARM_MIDDLE = 8,
        DROP_L_LEG_BOTTOM = 9,
        DROP_L_LEG_DEFAULT = 10,
        DROP_L_LEG_HIGH = 11,
        DROP_L_LEG_LOW = 12,
        DROP_L_LEG_MIDDLE = 13,
        DROP_R_ARM_DEFAULT = 14,
        DROP_R_ARM_HIGH = 15,
        DROP_R_ARM_LOW = 16,
        DROP_R_ARM_MIDDLE = 17,
        DROP_R_LEG_BOTTOM = 18,
        DROP_R_LEG_DEFAULT = 19,
        DROP_R_LEG_HIGH = 20,
        DROP_R_LEG_LOW = 21,
        DROP_R_LEG_MIDDLE = 22,
        NORMAL = 23,
        STAY_ACCESSORY_HAT = 24,
        STAY_BODY_BROKENHEADVARIATION = 25,
        STAY_BODY_HEAD = 26,
        STAY_BODY_INTESTINE = 27,
        STAY_BODY_LOWERBODY = 28,
        STAY_L_ARM_DEFAULT = 29,
        STAY_L_ARM_HIGH = 30,
        STAY_L_ARM_LOW = 31,
        STAY_L_ARM_MIDDLE = 32,
        STAY_L_LEG_BOTTOM = 33,
        STAY_L_LEG_DEFAULT = 34,
        STAY_L_LEG_HIGH = 35,
        STAY_L_LEG_LOW = 36,
        STAY_L_LEG_MIDDLE = 37,
        STAY_R_ARM_DEFAULT = 38,
        STAY_R_ARM_HIGH = 39,
        STAY_R_ARM_LOW = 40,
        STAY_R_ARM_MIDDLE = 41,
        STAY_R_LEG_BOTTOM = 42,
        STAY_R_LEG_DEFAULT = 43,
        STAY_R_LEG_HIGH = 44,
        STAY_R_LEG_LOW = 45,
        STAY_R_LEG_MIDDLE = 46,
    };
}
namespace app::ropeway::gui::PauseBehavior {
    enum class Result : int64_t {
        RETURN = 0,
        LOAD = 1,
        QUIT = 2,
    };
}
namespace via::motion::script::FootEffectController {
    enum class NoHitEmitEffectType : int64_t {
        Previous = 0,
        Joint = 1,
        None = 2,
    };
}
namespace app::ropeway::enemy::em5000 {
    enum class WeakPartsID : int64_t {
        UL_Body00 = 111,
        UL_Body01 = 114,
        UL_Body02 = 115,
        UR_Body03 = 106,
        UR_Body04 = 107,
        UR_Body05 = 108,
        DL_Body06 = 112,
        DL_Body07 = 116,
        DL_Body08 = 117,
        DR_Body09 = 109,
        DR_Body10 = 110,
        DR_Body11 = 113,
        UL_LeftArm00 = 103,
        UL_LeftArm01 = 104,
        UL_LeftArm02 = 105,
        UR_RightArm00 = 100,
        UR_RightArm01 = 101,
        UR_RightArm02 = 102,
        DL_LeftLeg00 = 121,
        DL_LeftLeg01 = 122,
        DL_LeftLeg02 = 123,
        DR_RightLeg00 = 118,
        DR_RightLeg01 = 119,
        DR_RightLeg02 = 120,
    };
}
namespace via::uvsequence {
    enum class UVTransform : int64_t {
        None = 0,
        Rotate90 = 1,
        Rotate180 = 2,
        Rotate270 = 3,
        Reverse = 4,
        ReverseRotate90 = 5,
        ReverseRotate180 = 6,
        ReverseRotate270 = 7,
    };
}
namespace app::ropeway::OptionManager {
    enum class OnOff : int64_t {
        ON = 0,
        OFF = 1,
    };
}
namespace app::ropeway {
    enum class ArmAdjustType : int64_t {
        NONE = 0,
        CANCEL = 1,
        FIT = 2,
    };
}
namespace via {
    enum class Country : int64_t {
        Unknown = 0xFFFFFFFF,
        Afghanistan = 4,
        AlandIslands = 248,
        Albania = 8,
        Algeria = 12,
        AmericanSamoa = 16,
        Andorra = 20,
        Angola = 24,
        Anguilla = 660,
        AntArtica = 10,
        AntiguaAndBarbuda = 28,
        Argentina = 32,
        Armenia = 51,
        Aruba = 533,
        Australia = 36,
        Austria = 40,
        Azerbaijan = 31,
        Bahamas = 44,
        BahraIN_ = 48,
        Bangladesh = 50,
        Barados = 52,
        Belarus = 112,
        Belgium = 56,
        Belize = 84,
        BenIN_ = 204,
        Bermuda = 60,
        Bhutan = 64,
        Bolivia = 68,
        Bonaire = 535,
        BosniaAndHerzegovina = 70,
        Botswana = 72,
        BouvetIsland = 74,
        Brazil = 76,
        BritishIndianOceanTerritory = 86,
        BruneiDarussalam = 96,
        Bulgaria = 100,
        BurkinaFaso = 854,
        Burundi = 108,
        CaboVerde = 132,
        Cambodia = 116,
        Cameroon = 120,
        Canada = 124,
        CaymanIslands = 136,
        CentralAfricanRepublic = 140,
        Chad = 148,
        Chile = 152,
        China = 156,
        ChristmasIsland = 162,
        CocosIslands = 166,
        Colombia = 170,
        Comoros = 174,
        Congo = 178,
        CongoDemocraticRepublic = 180,
        CookIsland = 184,
        CostaRica = 188,
        CotedDIVoire = 384,
        Croatia = 191,
        Cuba = 192,
        Curacao = 531,
        Cyprus = 196,
        CzechRepublic = 203,
        Denmark = 208,
        Djibouti = 262,
        Dominica = 212,
        DominicanRepublic = 214,
        Ecuador = 218,
        Egypt = 818,
        ElSalvador = 222,
        EquatorialGuinea = 226,
        Eritrea = 232,
        Estonia = 233,
        Ethiopia = 231,
        FalkanIslands = 236,
        FaroeIslands = 234,
        Fiji = 242,
        Finland = 246,
        France = 250,
        FrenchGuiana = 254,
        FrenchPolynesia = 258,
        FrenchSouthernTerritories = 260,
        Gabon = 266,
        Gambia = 270,
        Georgia = 268,
        Germany = 276,
        Ghana = 288,
        Gibraltar = 292,
        Greece = 300,
        Greenland = 304,
        Grenada = 308,
        Guadeloupe = 312,
        Guam = 316,
        Guatemala = 320,
        Guernsey = 831,
        Guinea = 324,
        GuineaBissau = 624,
        Guyana = 328,
        Haiti = 332,
        HeardIslandAndMcDonaldIslands = 334,
        Honduras = 340,
        HongKong = 344,
        Hungary = 348,
        Iceland = 352,
        India = 356,
        Indonesia = 360,
        Iran = 364,
        Iraq = 368,
        Ireland = 372,
        IsleOfMan = 833,
        Israel = 376,
        Italy = 380,
        Jamaica = 388,
        Japan = 392,
        Jersey = 832,
        Jordan = 400,
        Kazakhstan = 398,
        Kenya = 404,
        Kiribati = 296,
        NorthKorea = 408,
        SouthKorea = 410,
        Kuwait = 414,
        Kyrgyzstan = 417,
        Laos = 418,
        Latvia = 428,
        Lebanon = 422,
        Lesotho = 426,
        Liberia = 430,
        Libya = 434,
        LiechtensteIN_ = 438,
        Lithuania = 440,
        Luxembourg = 442,
        Macao = 446,
        Macedonia = 807,
        Madagascar = 450,
        Malawi = 454,
        Malaysia = 458,
        Maldives = 462,
        Mali = 466,
        Malta = 470,
        MarshallIslands = 584,
        Martinique = 474,
        Mauritania = 478,
        Mauritius = 480,
        Mayotte = 175,
        Mexico = 484,
        Micronesia = 583,
        Moldova = 498,
        Monaco = 492,
        Mongolia = 496,
        Montenegro = 499,
        Montserrat = 500,
        Morocco = 504,
        Mozambique = 508,
        Myanmar = 104,
        Namibia = 516,
        Nauru = 520,
        Nepal = 524,
        Netherlands = 528,
        NewCaledonia = 540,
        NewZealand = 554,
        Nicaragua = 558,
        Niger = 562,
        Nigeria = 566,
        Niue = 570,
        NorfolkIsland = 574,
        NorthernMarianaIsland = 580,
        Norway = 578,
        Oman = 512,
        Pakistan = 586,
        Palau = 585,
        Palestine = 275,
        Panama = 591,
        PapuaNewGuinea = 598,
        Paraguay = 600,
        Peru = 604,
        Philippines = 608,
        Pitcairn = 612,
        Poland = 616,
        Portugal = 630,
        PuertoRico = 630,
        Qatar = 634,
        Reunion = 638,
        Romania = 642,
        Russia = 643,
        Rwanda = 646,
        SaintBarthelemy = 652,
        SaintHelena = 654,
        SaintKittsAndNevis = 659,
        SaintLucia = 662,
        SaintMartIN_ = 663,
        SaintPierreAndMiquelon = 666,
        SaintVincentAndTheGrenadines = 670,
        Samoa = 882,
        SanMarino = 674,
        SaoTomeAndPrincipe = 678,
        SaudiArabia = 682,
        Senegal = 686,
        Serbia = 688,
        Seychelles = 690,
        SierraLeone = 694,
        Singapore = 702,
        SintMaarten = 534,
        Slovakia = 703,
        Slovenia = 705,
        SolomonIslands = 90,
        Somalia = 706,
        SouthAfrica = 710,
        SouthGeorgiaAndTheSouthSandwichIslands = 239,
        SouthSudan = 728,
        SpaIN_ = 724,
        SriLanka = 144,
        Sudan = 729,
        Suriname = 740,
        SvalbardAndJanMayen = 744,
        Swaziland = 756,
        Sweden = 752,
        Switzerland = 756,
        Syria = 760,
        Taiwan = 158,
        Tajikistan = 762,
        Tanzania = 834,
        Thailand = 764,
        TimorLeste = 626,
        Togo = 768,
        Tokelau = 772,
        Tonga = 776,
        TrinidadAndTobago = 780,
        Tunisia = 788,
        Turkey = 792,
        Turkmenistan = 795,
        TurksAndCalcosIslands = 796,
        Tuvalu = 798,
        Uganda = 800,
        Ukraine = 804,
        UnitedArabEmirates = 784,
        UnitedKingdom = 826,
        UnitedStatesMinorOutlyingIslands = 581,
        UnitedStatesOfAmerica = 840,
        Uruguay = 858,
        Uzbekistan = 860,
        Vanauatu = 548,
        VaticanCity = 336,
        Venezuela = 862,
        VietNam = 704,
        VirginIslandsBritish = 92,
        VirginIslandsUS = 850,
        WallisAndFutuna = 876,
        WesternSahara = 732,
        Yemen = 887,
        Zambia = 894,
        Zimbabwe = 716,
        Prchina = 156,
        Czech = 203,
        Hong_kong = 344,
        New_zealand = 554,
        South_korea = 410,
        United_states = 840,
        United_kingdom = 826,
        Uae = 784,
    };
}
namespace app::ropeway::gui::RogueInventoryBehavior {
    enum class CloseTrigger : int64_t {
        None = 0,
        InventoryButton = 1,
        MapButton = 2,
    };
}
namespace via::gui::detail {
    enum class ControlPointV : int64_t {
        Top = 0,
        Center = 1,
        Bottom = 2,
    };
}
namespace app::ropeway::EnvironmentStandbyManager {
    enum class Phase : int64_t {
        Standby = 0,
        OnWork = 1,
        ReleaseAllArea = 2,
        ReleaseAllLocation = 3,
        StartEnterReservation = 4,
        EnteringReservationMap = 5,
        EnteringReservationLocation = 6,
        KeepReservation = 7,
        StartRestoreFromReservation = 8,
        RestoringFromReservationScene = 9,
        RestoringFromReservationSetup = 10,
    };
}
namespace app::ropeway::environment::EnvironmentBoundaryClassifier {
    enum class Type : int64_t {
        Invalid = 0,
        TerraIN_ = 1,
        Prepare = 2,
    };
}
namespace via::effect::gpgpu::detail {
    enum class NodeBillboardType : int64_t {
        Bezier = 0,
        Spline = 1,
        Num = 2,
    };
}
namespace app::ropeway::fsmv2::WindowCheckCrash {
    enum class CheckType : int64_t {
        WINDOW_AND_OLDBARRI = 0,
        WINDOW_ONLY = 1,
        OLDBARRI_ONLY = 2,
    };
}
namespace System::IO {
    enum class SeekOrigin : int64_t {
        BegIN_ = 0,
        Current = 1,
        End = 2,
    };
}
namespace via::wwise::WwiseGlobalUserVariablesValue {
    enum class ComparisonOperator : int64_t {
        Equal = 0,
        NotEqual = 1,
        LessThan = 2,
        LessThanOrEqual = 3,
        GreaterThan = 4,
        GreaterThanOrEqual = 5,
        Between = 6,
    };
}
namespace app::ropeway::enemy::em3000 {
    enum class HearingSensorStopReason : int64_t {
        UNKNOWN = 0,
        ACTIVE = 1,
        SOUND_DETECTION_START = 2,
        SOUND_DETECTION_END = 3,
        LOSE_TARGET = 4,
    };
}
namespace app::ropeway::gui::RogueCharaSelectBehavior {
    enum class SubFlow : int64_t {
        INVALID = 0xFFFFFFFF,
        DEFAULT = 0,
        RESURRECTION = 1,
    };
}
namespace via::eq::Operator {
    enum class Type : int64_t {
        Exp = 0,
        Mul = 1,
        Div = 2,
        Mod = 3,
        Add = 4,
        Sub = 5,
    };
}
namespace via::hid {
    enum class DeviceKindDetails : int64_t {
        Unknown = 0,
        NULL_ = 1,
        MergedGamePad = 2,
        MergedKeyboard = 3,
        MergedMouse = 4,
        Dualshock4 = 5,
        DualShock4RemotePlay = 6,
        PSVitaRemotePlay = 7,
        XboxOne = 8,
        WindowsXInput = 9,
        WindowsGamingInput = 10,
        WindowsJoypad = 11,
        SwitchJoyCon = 12,
        SwitchProController = 13,
        SwitchDebugPad = 14,
        TouchScreenBuiltIN_ = 15,
        Keyboard = 16,
        Mouse = 17,
        PlayStationCamera = 18,
        Kinect = 19,
        Morpheus = 20,
        VirtualKeyboard = 21,
    };
}
namespace via::network::BattleInput {
    enum class Error : int64_t {
        None = 0,
        OpponentNotExist = 1,
        Checksum = 2,
        Timeout = 3,
        User = 4,
    };
}
namespace via::gui {
    enum class TextureAssetType : int64_t {
        UVSequence = 0,
        Texture = 1,
    };
}
namespace app::ropeway::OptionManager {
    enum class ControllerHoldOptionType : int64_t {
        NORMAL = 0,
        PRECISION = 1,
    };
}
namespace app::ropeway::enemy::em4400 {
    enum class ATTENTION_STATE : int64_t {
        NORMAL = 0,
        PANIC = 1,
        ESCAPE = 2,
    };
}
namespace app::ropeway::enemy::em4400 {
    enum class SET_TYPE : int64_t {
        NORMAL = 0,
        LOCKER_TRAY = 1,
    };
}
namespace via::render {
    enum class ColorSpace : int64_t {
        AUTO = 0,
        SRGB = 1,
        HDTV = 2,
        HDR10 = 3,
    };
}
namespace via::os {
    enum class MemorySource : int64_t {
        UserLand = 0,
        Develop = 1,
        KernelLand = 2,
        Default = 0,
    };
}
namespace app::ropeway::gui::ReticleBehavior {
    enum class SparkShotState : int64_t {
        INVALID = 0,
        WAIT = 1,
        DEFAULT = 2,
        BURST = 3,
        COOLDOWN = 4,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Press_Survivor = 0,
        Press_Enemy = 1,
        Damage = 2,
        AimTarget = 3,
        SoundMarker = 4,
        Sensor_Touch = 5,
        Sensor_Gimmick = 6,
        Sensor_Obstacle = 7,
        Marker_Avoid = 8,
        CameraDrawControl = 9,
        Tangle = 10,
        Attack_HoldFromWalk = 11,
        Attack_HoldFromCreep = 12,
        Attack_HoldLightly = 13,
        Attack_HoldOnBox = 14,
        Attack_Inertia_Hit = 15,
        Attack_Inertia = 16,
        Attack_BiteDirect = 17,
        Attack_BiteDirectFromCreep = 18,
        Attack_Tangle2Player = 19,
    };
}
namespace via::fsm {
    enum class SelectTiming : int64_t {
        Invalid = 0,
        BeforeAction = 1,
        AfterAction = 2,
    };
}
namespace via::storage {
    enum class BackgroundInstallSpeed : int64_t {
        Slow = 0,
        Suspend = 1,
        Fast = 2,
    };
}
namespace app::ropeway::survivor::fsmv2::condition::ActionOrderCondition {
    enum class WeaponListType : int64_t {
        None = 0,
        Include = 1,
        Exclude = 2,
        NONE = 0,
        INCLUDE = 1,
        EXCLUDE = 2,
    };
}
namespace via::motion::ChainResource {
    enum class ChainParamFlags : int64_t {
        ChianParamFlags_None = 0,
        ReflectEnviromental = 1,
    };
}
namespace via::wwise {
    enum class SinkTypePs4 : int64_t {
        MaIN_ = 0,
        MergeToMaIN_ = 1,
        Voice = 2,
        Personal = 3,
        PAD = 4,
        BGM = 5,
        Aux = 6,
        None = 7,
    };
}
namespace via::navigation::RelationMapInfo {
    enum class Operation : int64_t {
        CopyAttribute = 0,
        OperationNum = 1,
    };
}
namespace app::ropeway::gimmick::option::GimmickOptionEnemyTrapSettings {
    enum class StartType_Enum : int64_t {
        TIMER = 0,
        AREAIN_ = 1,
        DAMAGE = 2,
        FLAG = 3,
    };
}
namespace app::ropeway::gui::FloorMapSt11Behavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_1_601_0 = 1,
        Room_1_602_0 = 2,
        Room_1_603_0 = 3,
        Room_1_615_0 = 4,
        Room_1_632_0d = 5,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::SetMotionBlendRate {
    enum class SET_BLEND_TYPE : int64_t {
        FRONT_BACK = 0,
        LEFT_RIGHT = 1,
    };
}
namespace app::ropeway::effect::script::EPVExpertDamageEffectData {
    enum class DIRECTIONTYPE : int64_t {
        AttackerDirection = 0,
        InverseAttackerDirection = 1,
        DefenderCollisionNomal = 2,
        InverseDefenderCollisionNomal = 3,
        SlashDirection = 4,
        InverseSlashDirection = 5,
        ShotDirection = 6,
        InverseShotDirection = 7,
        Brun = 8,
        None = 9,
    };
}
namespace app::ropeway::ReNetTelemetry {
    enum class Phase : int64_t {
        Idle = 0,
        Move = 1,
        Convert = 2,
        Post = 3,
    };
}
namespace app::ropeway::RogueObjectSimpleController {
    enum class State : int64_t {
        INVALID = 0xFFFFFFFF,
        START = 0,
        WAIT = 1,
        UPDATE = 2,
        END = 3,
    };
}
namespace app::ropeway::gui::FileBehavior {
    enum class EndType : int64_t {
        DECIDE = 0,
        CANCEL = 1,
    };
}
namespace app::ropeway::fsmv2::enemy::stateaction::Em9000FsmAction_WarpList {
    enum class FlagCheckType : int64_t {
        TRUE_ = 0,
        FALSE_ = 1,
    };
}
namespace via::math {
    enum class FpClassify : int64_t {
        INFINITE_ = 0,
        Nan = 1,
        Normal = 2,
        SubNormal = 3,
        Zero = 4,
        Unknown = 5,
    };
}
namespace app::ropeway::gamemastering::RecordManager {
    enum class RANK : int64_t {
        INVALID = 0,
        SP = 1,
        S = 2,
        A = 3,
        B = 4,
        C = 5,
    };
}
namespace app::ropeway::gui::ItemCarKeyBehavior {
    enum class DemoStateEnum : int64_t {
        LampOff = 0,
        LampOn = 1,
    };
}
namespace app::ropeway::GPISwitch {
    enum class SlotType : int64_t {
        DipSwitch = 0,
        DebugCamera = 1,
        DrawFrameRate = 2,
        Dummy03 = 3,
        Dummy04 = 4,
        Dummy05 = 5,
        Dummy06 = 6,
        Dummy07 = 7,
    };
}
namespace app::ropeway::enemy::em6200::fsmv2::action::Em6200FsmAction_LadderAction {
    enum class ACTION_TYPE : int64_t {
        UP = 0,
        DOUN = 1,
    };
}
namespace via::render::RenderOutput {
    enum class OutputType : int64_t {
        Default = 0,
        Composite = 1,
    };
}
namespace via::motion {
    enum class InterpolationCurve : int64_t {
        Linear = 0,
        Smooth = 1,
        EaseIN_ = 2,
        EaseOut = 3,
    };
}
namespace via::render {
    enum class ShaderModifier : int64_t {
        None = 0,
        ClippingPlane = 1,
        Max = 2,
    };
}
namespace app::ropeway::survivor::npc::NpcOrderDefine::LookAtOrder {
    enum class ForceTargetKind : int64_t {
        Player = 0,
        Position = 1,
        Object = 2,
        Invalid = 3,
    };
}
namespace app::ropeway::enemy::em6000::fsmv2::action::Em6000FsmAction_SetOviductCollider {
    enum class EXEC_TYPE : int64_t {
        Always = 0,
        Sequence = 1,
    };
}
namespace via::render::detail {
    enum class ConstantBufferType : int64_t {
        None = 0,
        Polygon3D = 1,
        Polygon2D = 2,
        Mesh = 3,
        Lensflare = 4,
    };
}
namespace via::motion::IkDamageAction {
    enum class DamageTransition : int64_t {
        Non = 0,
        Impact = 1,
        Transition = 2,
        Damping = 3,
    };
}
namespace via::render::RenderTargetOperator {
    enum class Compress : int64_t {
        None = 0,
        Realtime = 1,
        Hardware = 65,
    };
}
namespace via::dynamics::BoxShape {
    enum class VoronoiId : int64_t {
        PlusX = 0,
        MinusX = 1,
        PlusY = 2,
        MinusY = 3,
        PlusZ = 4,
        MinusZ = 5,
        Max = 6,
    };
}
namespace via::motion::detail {
    enum class WindType : int64_t {
        None = 0,
        WindOrSpring = 1,
        GravityWave = 2,
        WindWave = 3,
    };
}
namespace app::ropeway::enemy::em0000::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_WAKEUP = 2,
        ACT_CLIMB_STEP = 3,
        ACT_FAKEDEAD = 4,
        ACT_KNOCK = 5,
        ACT_EAT = 6,
        ACT_ATTENTION_IDLE = 7,
        ACT_ATTENTION_WALK = 8,
        GMK_DOOR_INTERACT = 1000,
        GMK_CLIFF_FALL = 1001,
        ATK_HOLD = 2000,
        ATK_HOLD_DOWN = 2001,
        ATK_BITE_DIRECT = 2002,
        ATK_DEAD_INERTIA = 2003,
        ATK_BITE = 2004,
        ATK_BITE_CREEP = 2005,
        ATK_BITE_PUSHDOWN = 2006,
        ATK_BITE_FREEHAND = 2007,
        ATK_HOLD_CLIMB = 2008,
        ATK_HOLD_LIGHTLY = 2009,
        ATK_HOLD_DEAD_INERTIA = 2010,
        ATK_BITE_ONBOX = 2011,
        ATK_BITEDUAL_A = 2012,
        ATK_BITEDUAL_B = 2013,
        DMG_STUN = 3000,
        DMG_KNOCKBACK = 3001,
        DMG_DOWN = 3002,
        DMG_BURST = 3003,
        DMG_BROKEN_LEG = 3004,
        DMG_BROKEN_ARM = 3005,
        DMG_BROKEN_BODY = 3006,
        DMG_BUMP = 3007,
        DMG_DAMAGE_FROMDEAD = 3008,
        DMG_DOWN_ONSTAIRS = 3009,
        DMG_FLASH = 3010,
        DMG_SHOCK = 3011,
        DMG_SHOCK_DOWN = 3012,
        SPT_KNIFE = 4000,
        SPT_GRENADE = 4001,
        SPT_FLASHGRENADE = 4002,
        DIE_DEAD = 5000,
        DIE_BOMB = 5001,
        DIE_RIGID = 5002,
        SET_IDLE = 6000,
        SET_DEAD_IDLE = 6001,
        SET_FAKEDEAD = 6002,
        SET_KNOCK = 6003,
        SET_ONESHOT = 6004,
        SET_LOUNGE = 6005,
        SET_EAT = 6006,
        SET_WINDOW_IN_ = 6007,
        SET_DOOR_IN_ = 6008,
        SET_RAILING_FALL = 6009,
        SET_CAPTURE = 6010,
        SET_DEAD = 6011,
        SET_DEAD_ACTION = 6012,
        SET_EATEN = 6013,
        SET_EVENTSET = 6014,
        SET_RELOCATION = 6015,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace app::ropeway::gimmick::action::GimmickWordBlock {
    enum class BlockType : int64_t {
        SERPENT = 0,
        EAGLE = 1,
        JAGGER = 2,
        SUN = 3,
        MAX = 4,
    };
}
namespace app::ropeway::OperatorDefine::CheckOperator {
    enum class Type : int64_t {
        Equal = 0,
        NotEqual = 1,
    };
}
namespace via::timeline {
    enum class RestoreType : int64_t {
        Linear = 0,
        Descrete = 1,
    };
}
namespace app::ropeway::enemy::em3000::MotionPattern {
    enum class DownDead : int64_t {
        FaceUp = 0,
        FaceDown = 1,
    };
}
namespace app::ropeway::gamemastering::Event {
    enum class ID : int64_t {
        Invalid = 0xFFFFFFFF,
        EV_006_LNNN = 0,
        EV_010_LNNN = 1,
        GodaUI_0015 = 2,
        GodaUI_0017 = 3,
        GodaUI_0018 = 4,
        HanyuUI_0001 = 5,
        HanyuUI_0002 = 6,
        HanyuUI_0003 = 7,
        HanyuUI_0004 = 8,
        HanyuUI_0005 = 9,
        HanyuUI_0006 = 10,
        HanyuUI_0007 = 11,
        HanyuUI_0008 = 12,
        HanyuUI_0009 = 13,
        HanyuUI_0010 = 14,
        HanyuUI_0011 = 15,
        HanyuUI_0012 = 16,
        HanyuUI_0013 = 17,
        HanyuUI_0014 = 18,
        HanyuUI_0015 = 19,
        HanyuUI_0016 = 20,
        HanyuUI_0017 = 21,
        HanyuUI_0018 = 22,
        HanyuUI_0019 = 23,
        HanyuUI_0020 = 24,
        HanyuUI_0021 = 25,
        HanyuUI_0022 = 26,
        HanyuUI_0023 = 27,
        HanyuUI_0024 = 28,
        HanyuUI_0025 = 29,
        NakanoUI_0001 = 30,
        DevelopSubstituteEvent = 31,
        EV030_Test = 32,
        ShinzatoUI_0001 = 33,
        HayashiUI_0001 = 34,
        HayashiUI_0002 = 35,
        HayashiUI_0003 = 36,
        HayashiUI_0004 = 37,
        HayashiUI_0005 = 38,
        OkadaUI_0001 = 39,
        OkadaUI_0002 = 40,
        OkadaUI_0003 = 41,
        OkadaUI_0004 = 42,
    };
}
namespace app::ropeway::enemy::em6200::fsmv2::transition::Em6200MFsmTransitionEV_CheckActionDerivation {
    enum class CEHCK_TYPE : int64_t {
        WindowCrash = 0,
    };
}
namespace via::render::layer::PreDeferredLighting {
    enum class Segment : int64_t {
        ParticleCollision = 1,
    };
}
namespace app::ropeway::gimmick::option::GimmickOptionEnemyTrapSettings {
    enum class EndType_Enum : int64_t {
        TIMER = 0,
        AREAOUT = 1,
        DAMAGE = 2,
        FLAG = 3,
    };
}
namespace via::wwise {
    enum class SinkTypeNSW : int64_t {
        MaIN_ = 0,
        MergeToMaIN_ = 1,
        PlugIN_ = 2,
        None = 3,
    };
}
namespace via::Window::MessageArgs {
    enum class DeviceStatus : int64_t {
        Arrival = 0,
        Removal = 1,
    };
}
namespace via::storage::saveService {
    enum class SaveDataEncryptionPlatform : int64_t {
        None = 0,
        AutoUse = 1,
        All = 2,
    };
}
namespace app::ropeway::rogue::RogueCharacterDetailDataTable {
    enum class SetKind : int64_t {
        TypeA = 0,
        TypeB = 1,
        TypeC = 2,
        TypeD = 3,
        TypeE = 4,
        MAX = 5,
    };
}
namespace app::ropeway::gimmick::action::GimmickElectricStorageForG2Battle {
    enum class ColliderRequest : int64_t {
        TO_ENEMY = 0,
        TO_PLAYER = 1,
    };
}
namespace app::ropeway::PenetratePreventer {
    enum class PreventTargetFilter : int64_t {
        TerraIN_ = 0,
        Effect = 1,
        Dynamics = 2,
    };
}
namespace app::ropeway::Timer {
    enum class FunctionType : int64_t {
        OneShot = 0,
        Loop = 1,
    };
}
namespace app::ropeway::enemy::em7300::Em7300Think {
    enum class ThinkModeEnum : int64_t {
        NORMAL = 0,
    };
}
namespace app::ropeway {
    enum class IkLegKind : int64_t {
        TWO_LEG = 0,
        THREE_LEG = 1,
        FOUR_LEG = 2,
        DOG = 3,
    };
}
namespace app::ropeway::enemy::em6300::MotionPattern {
    enum class Threat : int64_t {
        Pattern0 = 0,
        Pattern1 = 1,
        Pattern2 = 2,
    };
}
namespace via::fsm {
    enum class SystemExecGroup : int64_t {
        Fsm = 0,
        MotionFsm = 1,
        SystemExecGroupNum = 2,
    };
}
namespace via::motion {
    enum class TransitionState : int64_t {
        None = 0,
        BegIN_ = 1,
        Setuped = 2,
        Update = 3,
        End = 4,
    };
}
namespace app::ropeway::gamemastering::RecordManager {
    enum class RecordId : int64_t {
        RECORD_001 = 0,
        RECORD_002 = 1,
        RECORD_003 = 2,
        RECORD_004 = 3,
        RECORD_005 = 4,
        RECORD_006 = 5,
        RECORD_007 = 6,
        RECORD_008 = 7,
        RECORD_009 = 8,
        RECORD_010 = 9,
        RECORD_011 = 10,
        RECORD_012 = 11,
        RECORD_013 = 12,
        RECORD_014 = 13,
        RECORD_015 = 14,
        RECORD_016 = 15,
        RECORD_017 = 16,
        RECORD_018 = 17,
        RECORD_019 = 18,
        RECORD_020 = 19,
        RECORD_021 = 20,
        RECORD_022 = 21,
        RECORD_023 = 22,
        RECORD_024 = 23,
        RECORD_025 = 24,
        RECORD_026 = 25,
        RECORD_027 = 26,
        RECORD_028 = 27,
        RECORD_029 = 28,
        RECORD_030 = 29,
        RECORD_031 = 30,
        RECORD_032 = 31,
        RECORD_033 = 32,
        RECORD_034 = 33,
        RECORD_035 = 34,
        RECORD_036 = 35,
        RECORD_037 = 36,
        RECORD_038 = 37,
        RECORD_039 = 38,
        RECORD_040 = 39,
        RECORD_041 = 40,
        RECORD_042 = 41,
        RECORD_043 = 42,
        RECORD_044 = 43,
        RECORD_045 = 44,
        RECORD_046 = 45,
        RECORD_047 = 46,
        RECORD_048 = 47,
        RECORD_049 = 48,
        RECORD_050 = 49,
        RECORD_051 = 50,
        RECORD_052 = 51,
        RECORD_053 = 52,
        RECORD_054 = 53,
        RECORD_055 = 54,
        RECORD_056 = 55,
        RECORD_057 = 56,
        RECORD_058 = 57,
        RECORD_059 = 58,
        RECORD_060 = 59,
        RECORD_061 = 60,
        RECORD_062 = 61,
        RECORD_063 = 62,
        RECORD_064 = 63,
        RECORD_065 = 64,
        RECORD_066 = 65,
        RECORD_067 = 66,
        RECORD_068 = 67,
        RECORD_069 = 68,
        RECORD_070 = 69,
        RECORD_071 = 70,
        RECORD_072 = 71,
        RECORD_073 = 72,
        RECORD_074 = 73,
        RECORD_075 = 74,
        RECORD_076 = 75,
        RECORD_077 = 76,
        RECORD_078 = 77,
        RECORD_079 = 78,
        RECORD_080 = 79,
        RECORD_081 = 80,
        RECORD_082 = 81,
        RECORD_083 = 82,
        RECORD_084 = 83,
        RECORD_085 = 84,
        RECORD_086 = 85,
        RECORD_087 = 86,
        RECORD_088 = 87,
        RECORD_089 = 88,
        RECORD_090 = 89,
        RECORD_091 = 90,
        MAX = 91,
    };
}
namespace via::motion::SupportPolygon {
    enum class SupportModeType : int64_t {
        None = 0,
        FootL = 1,
        FootR = 2,
        FootLR = 3,
    };
}
namespace app::ropeway::enemy::tracks::Em9999ColliderTrack {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Press = 0,
        Damage = 1,
        Sensor_Touch = 2,
        Attack = 3,
    };
}
namespace via::render::layer {
    enum class SolidSegment : int64_t {
        ZPrepass = 0,
        PrepareRoughTransparent = 1,
        DrawRoughTransparent = 2,
        ZIgnoreBegIN_ = 0,
        ZIgnorePrepassSolid = 1,
        ZIgnorePrepassTwoSide = 2,
        ZIgnorePrepassTwoSideAlphaTest = 3,
        ZIgnorePrepassAlphaTest = 4,
        ZIgnoreEnd = 5,
        SolidVfx = 6,
    };
}
namespace app::ropeway::gimmick::action::PositionFollower {
    enum class ParentType : int64_t {
        GIMMICK = 0,
        ENEMY = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickNumberLockSeized {
    enum class RoutineType : int64_t {
        WAIT = 0,
        SELECT = 1,
        PUT = 2,
    };
}
namespace app::ropeway::fsmv2::condition::OnMoveStick {
    enum class StickType : int64_t {
        ANY = 0,
        FORWARD = 1,
        BACK = 2,
        UP = 3,
        DOWN = 4,
        LEFT = 5,
        RIGHT = 6,
    };
}
namespace app::ropeway::gamemastering::RogueBaseAreaFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        GUI_CREATION_TO_BASE = 1,
        GUI_CREATION_TO_INGAME = 2,
        WAIT_GUI_CREATION_TO_BASE = 3,
        WAIT_GUI_CREATION_TO_INGAME = 4,
        UPDATE = 5,
        WAIT_FADE = 6,
        PRE_BASE_AREA = 7,
        BASE_AREA = 8,
        LOAD_AUTOSAVE = 9,
        TO_INGAME = 10,
        TO_EXTRAMENU = 11,
        FINALIZE = 12,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace via::effect::detail {
    enum class EmitterParticleType : int64_t {
        Unknown = 0,
        Billboard = 1,
        Mesh = 2,
        RibbonFollow = 3,
        RibbonLength = 4,
        RibbonChaIN_ = 5,
        FluidEmitter = 6,
        NodeBillboardGPU = 7,
        StrainRibbon = 8,
        NoDraw = 9,
        Polygon = 10,
        RibbonTrail = 11,
        PolygonTrail = 12,
        BillboardGPU = 13,
        RibbonFixEnd = 14,
        RibbonLightweight = 15,
        RibbonFollowGPU = 16,
        Lightning = 17,
    };
}
namespace app::ropeway::CameraTwirler {
    enum class UpdateStatus : int64_t {
        INVALID = 0,
        CONTROL = 1,
        FORCE = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class Em0000DirtyPreset_Pants : int64_t {
        Pants00 = 0,
        Pants01 = 1,
    };
}
namespace app::ropeway::IkHumanRetargetSolver {
    enum class SolverType : int64_t {
        None = 0,
        Hand = 1,
        AdjustHip = 2,
    };
}
namespace via::clr {
    enum class MetadataType : int64_t {
        Module = 0,
        TypeRef = 1,
        TypeDef = 2,
        FieldPtr = 3,
        Field = 4,
        MethodPtr = 5,
        MethodDef = 6,
        ParamPtr = 7,
        Param = 8,
        InterfaceImpl = 9,
        MemberRef = 10,
        Constant = 11,
        CustomAttribute = 12,
        FieldMarshal = 13,
        DeclSecurity = 14,
        ClassLayout = 15,
        FieldLayout = 16,
        StandAloneSig = 17,
        EventMap = 18,
        EventPtr = 19,
        Event = 20,
        PropertyMap = 21,
        PropertyPtr = 22,
        Property = 23,
        MethodSemantics = 24,
        MethodImpl = 25,
        ModuleRef = 26,
        TypeSpec = 27,
        ImplMap = 28,
        FieldRVA = 29,
        ENCLog = 30,
        ENCMap = 31,
        Assembly = 32,
        AssemblyProcessor = 33,
        AssemblyOS = 34,
        AssemblyRef = 35,
        AssemblyRefProcessor = 36,
        AssemblyRefOS = 37,
        File = 38,
        ExportedType = 39,
        ManifestResource = 40,
        NestedClass = 41,
        GenericParam = 42,
        MethodSpec = 43,
        GenericParamConstraint = 44,
        Max = 45,
        UserString = 112,
    };
}
namespace via::os {
    enum class ThreadPriority : int64_t {
        TimeCritical = 0,
        Highest = 1,
        AboveNormal = 2,
        Normal = 3,
        BelowNormal = 4,
        Lowest = 5,
        Idle = 6,
        NumOfPriority = 7,
    };
}
namespace app::ropeway::gimmick::option::SurvivorControlSettings::Param {
    enum class CastType : int64_t {
        Player = 0,
        Npc = 1,
        Actor = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickMobileBookShelf {
    enum class MoveType : int64_t {
        LEFT = 0,
        RIGHT = 1,
    };
}
namespace app::ropeway::enemy::em7200::fsmv2::condition::TargetDirection {
    enum class CompareType : int64_t {
        Equal = 0,
        NotEqual = 1,
        Less = 2,
        LessEq = 3,
        Greater = 4,
        GreaterEq = 5,
    };
}
namespace app::ropeway::gamemastering::item::DetailCollisionBase {
    enum class DispAttr : int64_t {
        All = 0,
        LeonOnly = 1,
        ClaireOnly = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickWindow {
    enum class ChildType : int64_t {
        WINDOW_BEFORE_BREAK = 0,
        WINDOW_BREAKING = 1,
        WINDOW_AFTER_BREAK = 2,
        OLD_BARRICADE_BEFORE_BREAK = 3,
        OLD_BARRICADE_BREAKING = 4,
        OLD_BARRICADE_AFTER_BREAK = 5,
        BARRICADE = 6,
        NUM = 7,
    };
}
namespace via::network::session {
    enum class SearchKeyAttr : int64_t {
        None = 0,
        SameRegion = 1,
    };
}
namespace via::render::RenderLayer {
    enum class Priority : int64_t {
        TopMost = 16777216,
        Top = 50331648,
        Default = 83886080,
        Bottom = 117440512,
        BottomMost = 150994944,
    };
}
namespace app::ropeway::enemy::em0000::MotionPattern {
    enum class WindowKnockStart : int64_t {
        FromF = 0,
        FromL = 1,
        FromR = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickCustomJackControl::Transition {
    enum class TriggerKeyButtonType : int64_t {
        ACTION = 0,
        WALK = 1,
    };
}
namespace via::motion {
    enum class ExpressionUpdate : int64_t {
        MotionBegIN_ = 0,
        MotionEnd = 1,
        ConstraintsBegIN_ = 2,
        ConstraintsEnd = 3,
        Last = 4,
        ByBehavior = 5,
    };
}
namespace app::ropeway::fsmv2::JackStateEndAction {
    enum class MotionEndMethodType : int64_t {
        StateEndOfMotion = 0,
        StateEndOrNextEndOfMotion = 1,
        StateNextEndOfMotion = 2,
        PrevStateEndOfMotion = 3,
    };
}
namespace via::PlaneXZ {
    enum class NGLevel : int64_t {
        None = 0,
        NanFinite = 1,
        Assert = 2,
        Default = 1,
    };
}
namespace via::hid {
    enum class VibrationOutputNode : int64_t {
        Dual = 0,
        LeftOnly = 1,
        RightOnly = 2,
        LeftAndRightSeparately = 3,
    };
}
namespace via::hid {
    enum class GamePadButton : int64_t {
        LUp = 1,
        LDown = 2,
        LLeft = 4,
        LRight = 8,
        RUp = 16,
        RDown = 32,
        RLeft = 64,
        RRight = 128,
        LTrigTop = 256,
        LTrigBottom = 512,
        RTrigTop = 1024,
        RTrigBottom = 2048,
        LStickPush = 4096,
        RStickPush = 8192,
        CLeft = 16384,
        CRight = 32768,
        CCenter = 65536,
        Decide = 131072,
        Cancel = 262144,
        PlatformHome = 524288,
        EmuLup = 1048576,
        EmuLright = 2097152,
        EmuLdown = 4194304,
        EmuLleft = 8388608,
        EmuRup = 16777216,
        EmuRright = 33554432,
        EmuRdown = 67108864,
        EmuRleft = 134217728,
        LSL = 268435456,
        LSR = 536870912,
        RSL = 1073741824,
        RSR = 2147483648,
        None = 0,
        All = 0xFFFFFFFF,
    };
}
namespace app::ropeway::SurvivorAssignManager {
    enum class CastingRequestResult : int64_t {
        Accepted = 0,
        Refused = 1,
    };
}
namespace app::ropeway::GameRankSystem {
    enum class RankNo : int64_t {
        Rank0 = 0,
        Rank1 = 1,
        Rank2 = 2,
        Rank3 = 3,
        Rank4 = 4,
        Rank5 = 5,
        Rank6 = 6,
        Rank7 = 7,
        Rank8 = 8,
        Rank9 = 9,
        Rank10 = 10,
        Rank11 = 11,
        Rank12 = 12,
        Rank13 = 13,
        Rank14 = 14,
        Rank15 = 15,
        Rank16 = 16,
        Rank17 = 17,
        Rank18 = 18,
        Rank19 = 19,
        RankEnumALL = 20,
    };
}
namespace app::ropeway::enemy::em6200 {
    enum class MOTION_PATTERN : int64_t {
        TargetLR = 0,
        FlipLR = 1,
        Search = 2,
        Provoke = 3,
        WalkStart = 4,
        Stun = 5,
        Knockback = 6,
        AddDamage = 7,
        Flash = 8,
        GrappleToFlash = 9,
    };
}
namespace via::clr {
    enum class EnumI4 : int64_t {
        Dummy = 0,
    };
}
namespace via::effect::detail {
    enum class ExpressionOperator : int64_t {
        Addition = 0,
        Subtraction = 1,
        Multiplication = 2,
        Division = 3,
        Assign = 4,
        ForceWord = 0xFFFFFFFF,
    };
}
namespace via::effect::script::Collision::ReLibEffectHitController::DamageInfo {
    enum class Scale : int64_t {
        L = 0,
        M = 1,
        S = 2,
    };
}
namespace app::ropeway::gui::BonusBehavior {
    enum class Mode : int64_t {
        ROOT = 0,
        RECORD = 1,
        RECORD_ROGUE = 2,
        COSTUME = 3,
        CONCEPTART = 4,
        FIGURE = 5,
        CLEAR_STATE = 6,
    };
}
namespace via::motion::IkMultipleDamageAction {
    enum class DebugDamageTransition : int64_t {
        Non = 0,
        ImpactState = 1,
        MaxState = 2,
    };
}
namespace via::motion::tree {
    enum class LinkType : int64_t {
        Unknown = 0,
        Motion = 1,
        Param = 2,
    };
}
namespace via::render {
    enum class ShadowCastFlag : int64_t {
        A = 1,
        B = 2,
        ALL = 3,
    };
}
namespace via::navigation::AIMapEffectorCore {
    enum class ExtraLinkCreationResult : int64_t {
        Success = 0,
        Fail = 1,
        Retry = 2,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class Eaten : int64_t {
        EAT_CORPSE_00 = 0,
        EAT_CORPSE_01 = 1,
        EAT_CORPSE_02 = 2,
    };
}
namespace via::render::command {
    enum class TypeId : int64_t {
        Clear = 0,
        CopyResource = 1,
        DrawIndexed = 2,
        Draw = 3,
        DrawIndexedInstanced = 4,
        DrawIndexedInstancedIndirect = 5,
        DrawInstanced = 6,
        MultiDrawIndexedInstancedIndirect = 7,
        MultiDrawIndexedInstancedIndirectCount = 8,
        Dispatch = 9,
        DispatchIndirect = 10,
        UpdateConstantBuffer = 11,
        UpdateBuffer = 12,
        Marker = 13,
        DepthBoundsTest = 14,
        RecordedCommand = 15,
        AsyncDispatch = 16,
        Fence = 17,
        XB1Extention = 18,
        PS4Extention = 19,
        NVNExtention = 20,
    };
}
namespace app::ropeway::enemy::em0000::MotionPattern {
    enum class PivotTurn : int64_t {
        L90 = 0,
        R90 = 1,
        L180 = 2,
        R180 = 3,
    };
}
namespace app::ropeway::enemy::em3000::tracks::Em3000HoldDamageTrack {
    enum class HoldDamageType : int64_t {
        HOLD = 0,
    };
}
namespace via::network::storage {
    enum class Target : int64_t {
        None = 0,
        Local = 1,
        Native = 2,
    };
}
namespace app::ropeway::enemy::em3000::fsmv2::action::Em3000FsmAction_Turn {
    enum class TurnType : int64_t {
        L_90 = 0,
        L_180 = 1,
        R_90 = 2,
        R_180 = 3,
    };
}
namespace app::ropeway::gamemastering::ScenarioSequenceManager {
    enum class ManageType : int64_t {
        Enemy = 0,
        Fsm = 1,
        Scenario = 2,
        Item = 3,
        SearchText = 4,
        Sound = 5,
        File = 6,
        Invalid = 7,
    };
}
namespace app::ropeway::fsmv2::DrawerDummyItemInstanciate {
    enum class Mode : int64_t {
        Instanciate = 0,
        Destroy = 1,
    };
}
namespace via::behaviortree {
    enum class ExecGroup : int64_t {
        ExecGroup_00 = 0,
        ExecGroup_01 = 1,
        ExecGroup_02 = 2,
        ExecGroup_03 = 3,
        ExecGroup_04 = 4,
        ExecGroup_05 = 5,
        ExecGroup_06 = 6,
        ExecGroup_07 = 7,
    };
}
namespace via::clr {
    enum class IL : int64_t {
        IL_nop = 0,
        IL_break = 1,
        IL_ldarg_0 = 2,
        IL_ldarg_1 = 3,
        IL_ldarg_2 = 4,
        IL_ldarg_3 = 5,
        IL_ldloc_0 = 6,
        IL_ldloc_1 = 7,
        IL_ldloc_2 = 8,
        IL_ldloc_3 = 9,
        IL_stloc_0 = 10,
        IL_stloc_1 = 11,
        IL_stloc_2 = 12,
        IL_stloc_3 = 13,
        IL_ldarg_s = 14,
        IL_ldarga_s = 15,
        IL_starg_s = 16,
        IL_ldloc_s = 17,
        IL_ldloca_s = 18,
        IL_stloc_s = 19,
        IL_ldNULL_ = 20,
        IL_ldc_i4_m1 = 21,
        IL_ldc_i4_0 = 22,
        IL_ldc_i4_1 = 23,
        IL_ldc_i4_2 = 24,
        IL_ldc_i4_3 = 25,
        IL_ldc_i4_4 = 26,
        IL_ldc_i4_5 = 27,
        IL_ldc_i4_6 = 28,
        IL_ldc_i4_7 = 29,
        IL_ldc_i4_8 = 30,
        IL_ldc_i4_s = 31,
        IL_ldc_i4 = 32,
        IL_ldc_i8 = 33,
        IL_ldc_r4 = 34,
        IL_ldc_r8 = 35,
        IL_0x24 = 36,
        IL_dup = 37,
        IL_pop = 38,
        IL_jmp = 39,
        IL_call = 40,
        IL_calli = 41,
        IL_ret = 42,
        IL_br_s = 43,
        IL_brfalse_s = 44,
        IL_brtrue_s = 45,
        IL_beq_s = 46,
        IL_bge_s = 47,
        IL_bgt_s = 48,
        IL_ble_s = 49,
        IL_blt_s = 50,
        IL_bne_un_s = 51,
        IL_bge_un_s = 52,
        IL_bgt_un_s = 53,
        IL_ble_un_s = 54,
        IL_blt_un_s = 55,
        IL_br = 56,
        IL_brFALSE_ = 57,
        IL_brTRUE_ = 58,
        IL_beq = 59,
        IL_bge = 60,
        IL_bgt = 61,
        IL_ble = 62,
        IL_blt = 63,
        IL_bne_un = 64,
        IL_bge_un = 65,
        IL_bgt_un = 66,
        IL_ble_un = 67,
        IL_blt_un = 68,
        IL_switch = 69,
        IL_ldind_i1 = 70,
        IL_ldind_u1 = 71,
        IL_ldind_i2 = 72,
        IL_ldind_u2 = 73,
        IL_ldind_i4 = 74,
        IL_ldind_u4 = 75,
        IL_ldind_i8 = 76,
        IL_ldind_i = 77,
        IL_ldind_r4 = 78,
        IL_ldind_r8 = 79,
        IL_ldind_ref = 80,
        IL_stind_ref = 81,
        IL_stind_i1 = 82,
        IL_stind_i2 = 83,
        IL_stind_i4 = 84,
        IL_stind_i8 = 85,
        IL_stind_r4 = 86,
        IL_stind_r8 = 87,
        IL_add = 88,
        IL_sub = 89,
        IL_mul = 90,
        IL_div = 91,
        IL_div_un = 92,
        IL_rem = 93,
        IL_rem_un = 94,
        IL_and = 95,
        IL_or = 96,
        IL_xor = 97,
        IL_shl = 98,
        IL_shr = 99,
        IL_shr_un = 100,
        IL_neg = 101,
        IL_not = 102,
        IL_conv_i1 = 103,
        IL_conv_i2 = 104,
        IL_conv_i4 = 105,
        IL_conv_i8 = 106,
        IL_conv_r4 = 107,
        IL_conv_r8 = 108,
        IL_conv_u4 = 109,
        IL_conv_u8 = 110,
        IL_callvirt = 111,
        IL_cpobj = 112,
        IL_ldobj = 113,
        IL_ldstr = 114,
        IL_newobj = 115,
        IL_castclass = 116,
        IL_isinst = 117,
        IL_conv_r_un = 118,
        IL_unbox = 121,
        IL_throw = 122,
        IL_ldfld = 123,
        IL_ldflda = 124,
        IL_stfld = 125,
        IL_ldsfld = 126,
        IL_ldsflda = 127,
        IL_stsfld = 128,
        IL_stobj = 129,
        IL_conv_ovf_i1_un = 130,
        IL_conv_ovf_i2_un = 131,
        IL_conv_ovf_i4_un = 132,
        IL_conv_ovf_i8_un = 133,
        IL_conv_ovf_u1_un = 134,
        IL_conv_ovf_u2_un = 135,
        IL_conv_ovf_u4_un = 136,
        IL_conv_ovf_u8_un = 137,
        IL_conv_ovf_i_un = 138,
        IL_conv_ovf_u_un = 139,
        IL_box = 140,
        IL_newarr = 141,
        IL_ldlen = 142,
        IL_ldelema = 143,
        IL_ldelem_i1 = 144,
        IL_ldelem_u1 = 145,
        IL_ldelem_i2 = 146,
        IL_ldelem_u2 = 147,
        IL_ldelem_i4 = 148,
        IL_ldelem_u4 = 149,
        IL_ldelem_i8 = 150,
        IL_ldelem_i = 151,
        IL_ldelem_r4 = 152,
        IL_ldelem_r8 = 153,
        IL_ldelem_ref = 154,
        IL_stelem_i = 155,
        IL_stelem_i1 = 156,
        IL_stelem_i2 = 157,
        IL_stelem_i4 = 158,
        IL_stelem_i8 = 159,
        IL_stelem_r4 = 160,
        IL_stelem_r8 = 161,
        IL_stelem_ref = 162,
        IL_ldelem = 163,
        IL_stelem = 164,
        IL_unbox_any = 165,
        IL_conv_ovf_i1 = 179,
        IL_conv_ovf_u1 = 180,
        IL_conv_ovf_i2 = 181,
        IL_conv_ovf_u2 = 182,
        IL_conv_ovf_i4 = 183,
        IL_conv_ovf_u4 = 184,
        IL_conv_ovf_i8 = 185,
        IL_conv_ovf_u8 = 186,
        IL_refanyval = 194,
        IL_ckfinite = 195,
        IL_mkrefany = 198,
        IL_ldtoken = 208,
        IL_conv_u2 = 209,
        IL_conv_u1 = 210,
        IL_conv_i = 211,
        IL_conv_ovf_i = 212,
        IL_conv_ovf_u = 213,
        IL_add_ovf = 214,
        IL_add_ovf_un = 215,
        IL_mul_ovf = 216,
        IL_mul_ovf_un = 217,
        IL_sub_ovf = 218,
        IL_sub_ovf_un = 219,
        IL_endfinally = 220,
        IL_leave = 221,
        IL_leave_s = 222,
        IL_stind_i = 223,
        IL_conv_u = 224,
        IL_arglist = 65024,
        IL_ceq = 65025,
        IL_cgt = 65026,
        IL_cgt_un = 65027,
        IL_clt = 65028,
        IL_clt_un = 65029,
        IL_ldftn = 65030,
        IL_ldvirtftn = 65031,
        IL_ldarg = 65033,
        IL_ldarga = 65034,
        IL_starg = 65035,
        IL_ldloc = 65036,
        IL_ldloca = 65037,
        IL_stloc = 65038,
        IL_localloc = 65039,
        IL_endfilter = 65041,
        IL_unaligned = 65042,
        IL_volatile = 65043,
        IL_tail = 65044,
        IL_initobj = 65045,
        IL_constrained = 65046,
        IL_cpblk = 65047,
        IL_initblk = 65048,
        IL_rethrow = 65050,
        IL_sizeof = 65052,
        IL_refanytype = 65053,
        IL_readonly = 65054,
    };
}
namespace via::timeline {
    enum class TimelineState : int64_t {
        Play = 0,
        Pause = 1,
    };
}
namespace via {
    enum class GameLiveStreamingStatus : int64_t {
        Disable = 0xFFFFFFFF,
        Failed = -2,
        Stop = 0,
        OnAir = 1,
    };
}
namespace app::ropeway::gui::CostumeBehavior {
    enum class CategoryIndex : int64_t {
        SCENARIO = 0,
        CLASSIC = 1,
        CUSTOM = 2,
        MAX = 3,
        INVALID = 4,
    };
}
namespace app::ropeway::gamemastering::UIMapManager {
    enum class MapDispState : int64_t {
        NONE = 0,
        OPENED = 1,
        ENTRIED = 2,
    };
}
namespace app::ropeway::CollisionDefine {
    enum class AttackTypes : int64_t {
        None = 0,
        Shot = 1,
        ExShot = 2,
        Slash = 3,
        Stab = 4,
        Throw = 5,
        Explosion = 6,
        Burn = 7,
        BurnUp = 8,
        Flame = 9,
        Acid = 10,
        SparkS = 11,
        SparkL = 12,
        Extra = 13,
    };
}
namespace via::network::AutoMatchmaking {
    enum class Phase : int64_t {
        Idle = 0,
        Host_Init = 1,
        Host_SetRule = 2,
        Host_CreateSession = 3,
        Host_CreateWait = 4,
        Host_InGame = 5,
        Host_Timeout = 6,
        Guest_Init = 7,
        Guest_CreateSession = 8,
        Guest_CreateWait = 9,
        Guest_SetSearchRule = 10,
        Guest_SearchSession = 11,
        Guest_SearchWait = 12,
        Guest_ThinkRule = 13,
        Guest_GiveupSession = 14,
        Guest_GiveupWait = 15,
        Guest_JoinSession = 16,
        Guest_JoinWait = 17,
        Guest_InGame = 18,
        Guest_SearchInterval = 19,
        Guest_Timeout = 20,
        Guest_TimeoutWait = 21,
        Guest_XboxSmartMatch = 22,
        Guest_XboxSmartMatchWait = 23,
        Guest_XboxSmartMatchHoge = 24,
    };
}
namespace app::ropeway::gimmick::action::GimmickWristTagReader {
    enum class OptionType : int64_t {
        LOCK_A = 0,
        LOCK_B = 1,
        LOCK_C = 2,
        LOCK_D = 3,
    };
}
namespace app::ropeway::enemy::em4400 {
    enum class TRAY_REACTION_TYPE : int64_t {
        NONE = 0,
        SHORT_MOVE = 1,
        ESCAPE = 2,
    };
}
namespace app::ropeway::enemy::em7200::Em7200Think {
    enum class ThinkModeEnum : int64_t {
        OMOTENASI = 0,
        HONKI_LV1 = 1,
        HONKI_LV2 = 2,
    };
}
namespace app::ropeway::standby::LoadPriorityDefine {
    enum class Priority : int64_t {
        GUI_TITLE = -9030,
        GUI_INGAME = -9020,
        GUI_ITEM = -9000,
        LOCATION_ROOT = -8800,
        LOCATION = -8700,
        LOCATION_SUBFOLDER = -8600,
        GD_LOAD = -8200,
        PLAYER = -7000,
        ASSET_PACKAGE = -6001,
        ENEMY_ACTIVE = -6000,
        MAPSCENE_ADDITIONAL = -5001,
        MAPSCENE_HIGHEST = -5000,
        MAPSCENE_HIGHER_DYNAMIC_STANDARD = -4950,
        MAPSCENE_DYNAMNIC_HIGHEST = -4900,
        TIMELINE_CURRENT_MAP_HIGHER = -4897,
        TIMELINE_CURRENT_MAP = -4895,
        MAPSCENE_DYNAMIC_ACTIVE_HIGHEST = -4890,
        MAPSCENE_SCENARIO = -4700,
        MAPSCENE_DYNAMIC_ACTIVE_LOWEST = -4501,
        TIMELINE_ACTIVE = -4500,
        MAPSCENE_DYNAMNIC_STANDBY_HIGHEST = -4399,
        MAPSCENE_DYNAMIC_STANDBY_LOWEST = -4101,
        MAPSCENE_DYNAMIC_LOWEST = -4100,
        MAPSCENE_LOWER_DYNAMIC_STANDARD = -4050,
        MAPSCENE_LOWEST = -4000,
        TIMELINE = -3000,
        LOCATION_BACKLOAD = -2200,
        DEFAULT = -2199,
    };
}
namespace app::ropeway::TerrainAnalyzer {
    enum class Attribute : int64_t {
        ASCEND = 0,
        DESCEND = 1,
        IGNORE_PLAYER = 2,
        IGNORE_ENEMY = 3,
        ENEMY_FALL = 4,
    };
}
namespace app::ropeway::EnemyManager {
    enum class WwiseEnemyExistIndex : int64_t {
        ALL = 0,
        EM0000 = 1,
        EM3000 = 2,
        EM4000 = 3,
        EM5000 = 4,
        EM6000 = 5,
        EM6200 = 6,
        EM6300 = 7,
        EM7000 = 8,
        EM7100 = 9,
        EM7200 = 10,
        EM7300 = 11,
        EM7400 = 12,
        EM9000 = 13,
        NUM = 14,
    };
}
namespace app::ropeway::enemy::em0000::Em0000DropParts {
    enum class DelayBreakStatus : int64_t {
        Invalid = 0xFFFFFFFF,
        Hang = 0,
        KeyFramed = 1,
        Dynamic = 2,
    };
}
namespace app::ropeway::enemy::userdata::Em9000PlayerFindUserData {
    enum class PlayerFindType : int64_t {
        Catch = 0,
        HandLight = 1,
        BackFind = 2,
    };
}
namespace app::ropeway::gui::RogueInventorySlotBehavior {
    enum class GuideType : int64_t {
        NoDisp = 0,
        ItemMove = 1,
        ItemSelect = 2,
        ItemUse = 3,
        ItemCombine = 4,
        ItemExCombine = 5,
        ItemBoxIN_ = 6,
        ItemBoxOut = 7,
        ItemBoxInPos = 8,
        ItemBoxOutPos = 9,
        ShortcutMove = 10,
        CommandExchange = 11,
        CommandUse = 12,
        CommandCombine = 13,
        CommandSearch = 14,
        CommandRemove = 15,
        CommandShortcut = 16,
        CommandEquip = 17,
        CommandRemoveEquip = 18,
        CommandEquipSub = 19,
        CommandRemoveEquipSub = 20,
        CommandCustomOut = 21,
        CommandGetItem = 22,
        CommandItemBoxIN_ = 23,
        CommandItemBoxOut = 24,
        CommandItemBoxInNum = 25,
        CommandItemBoxOutNum = 26,
        Next = 27,
    };
}
namespace app::ropeway::behaviortree::condition::BtCondition_PlayerState {
    enum class State : int64_t {
        IDLE = 0,
        WALK = 1,
        JOG = 2,
        HOLD = 3,
        CREEPING = 4,
        REACTION = 5,
        ATTACKED = 6,
    };
}
namespace via::wwise {
    enum class PhysicsValueType : int64_t {
        LinearVelocity = 0,
        AngularVelocity = 1,
        FrictionFactor = 2,
        Momentum = 3,
        AngularMomemtum = 4,
        MassTimesFrictionFactor = 5,
    };
}
namespace app::ropeway::survivor::npc::moveline {
    enum class PositionTypeKind : int64_t {
        Start = 0,
        Course = 1,
        Anchor = 2,
        End = 3,
        Invalid = 4,
    };
}
namespace app::ropeway::PlayerLightSwitchZone {
    enum class Priority : int64_t {
        LOW = 0,
        DEFAULT = 1,
        HIGH = 2,
    };
}
namespace via::nnfc::nfp {
    enum class FontRegion : int64_t {
        JpUsEu = 0,
        China = 1,
        Korea = 2,
        Taiwan = 3,
        Count = 4,
        Term = 4,
        MIN_ = 0,
        Max = 3,
    };
}
namespace via::relib::playlog::JsonWriter {
    enum class ValueType : int64_t {
        Unknown = 0,
        Object = 1,
        Array = 2,
        String = 3,
        Int = 4,
        Float = 5,
        TRUE_ = 6,
        FALSE_ = 7,
        NULL_ = 8,
    };
}
namespace app::ropeway::MapAreaDefineTable::MapAreaDefine {
    enum class PriorityLevel : int64_t {
        Low = 0,
        Normal = 1,
        High = 2,
    };
}
namespace app::ropeway::enemy::em6300::MotionPattern {
    enum class Step : int64_t {
        Left = 0,
        Right = 1,
        Left_B = 2,
        Right_B = 3,
        Back = 4,
    };
}
namespace via::effect::gpgpu::DeformedMesh::MeshCsResource {
    enum class SkinningShaderType : int64_t {
        SkinningShaderType_None = 0,
        SkinningShaderType_None_StorePrimaryUV = 1,
        SkinningShaderType_None_StoreSecondaryUV = 2,
        SkinningShaderType_None_StorePrimarySecondaryUV = 3,
        SkinningShaderType_4w = 4,
        SkinningShaderType_4w_StorePrimaryUV = 5,
        SkinningShaderType_4w_StoreSecondaryUV = 6,
        SkinningShaderType_4w_StorePrimarySecondaryUV = 7,
        SkinningShaderType_8w = 8,
        SkinningShaderType_8w_StorePrimaryUV = 9,
        SkinningShaderType_8w_StoreSecondaryUV = 10,
        SkinningShaderType_8w_StorePrimarySecondaryUV = 11,
        SkinningShaderType_Max = 12,
    };
}
namespace app::ropeway {
    enum class ExtraRegionType : int64_t {
        Unknown = 0,
        German = 1,
    };
}
namespace app::ropeway::weapon::generator {
    enum class BulletShotGunType : int64_t {
        Center = 0,
        Arround = 1,
    };
}
namespace app::ropeway::effect::script::PlRainEffect {
    enum class MAT_STATE : int64_t {
        AllDry = 0,
        Drying = 1,
        Wetting = 2,
        AllWet = 3,
    };
}
namespace via::collision {
    enum class Axis : int64_t {
        X = 0,
        Y = 1,
        Z = 2,
        Num = 3,
    };
}
namespace app::ropeway::NPCAccessManager {
    enum class AccessType : int64_t {
        PASS_THROUGH = 0,
        CLIMB_UP = 1,
        CLIMB_DOWN = 2,
        MAX = 3,
    };
}
namespace app::ropeway::SurvivorAssignManager {
    enum class OfferMode : int64_t {
        Normal = 0,
        OrderUnderVacant = 1,
    };
}
namespace app::ropeway::timeline::TimelineEventSettingTable::EnemySetting {
    enum class RequestType : int64_t {
        CallAction = 0,
        MotionJack = 1,
        KeepEvent = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickGoddessStatue {
    enum class StateType : int64_t {
        MEDAL = 0,
        CLOSED = 1,
        OPENED = 2,
    };
}
namespace app::Collision::CollisionSystem {
    enum class AsyncCastRayState : int64_t {
        Unknown = 0,
        NotYet = 1,
        TRUE_ = 2,
        FALSE_ = 3,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::EmCommonFsmAction_SetMotionVariationPatten {
    enum class SetTiming : int64_t {
        OnStart = 0,
        OnEnd = 1,
    };
}
namespace via::motion::Fsm2ActionPlayMotion {
    enum class FrameControl : int64_t {
        Normal = 0,
        SyncBaseLayer = 1,
        PauseStartFrame = 2,
        PauseEndFrame = 3,
    };
}
namespace via::gui {
    enum class InputListType : int64_t {
        None = 0,
        UpDown = 1,
        LeftRight = 2,
        LBRB = 3,
        Max = 4,
    };
}
namespace via::behaviortree::TreeNode {
    enum class WorkFlags : int64_t {
        IsNotifiedEnd = 1,
        HasEvaluated = 2,
        HasSelected = 4,
        IsCalledActionPrestart = 8,
        IsCalledActionStart = 16,
        IsNotifiedUnderLayerEnd = 32,
        IsBranchState = 64,
        IsEndState = 128,
        IsStartedSelector = 256,
        OverridedSelector = 512,
        DuplicatedAction = 1024,
    };
}
namespace via::effect::script::EffectLoadZone {
    enum class Status : int64_t {
        None = 0,
        ActiveTrigger = 1,
        Active = 2,
        InactiveTrigger = 3,
        Inactive = 4,
    };
}
namespace app::ropeway::gui::NewInventoryBehavior {
    enum class CallOpenMode : int64_t {
        Normal = 0,
        Map = 1,
        GetMap = 2,
        Map4th = 3,
        GetItem = 4,
        GetItemShortcut = 5,
        UseItem = 6,
        ItemBox = 7,
    };
}
namespace app::ropeway::ChoreographPlayer::ChoreographMovePosition::Rotation {
    enum class RotType : int64_t {
        ONSET = 0,
        SELECT = 1,
        RELATIVE_ = 2,
    };
}
namespace via::effect::script::EffectCommonDefine {
    enum class EffectEndType : int64_t {
        Kill = 0,
        Finish = 1,
    };
}
namespace via::motion {
    enum class BlendMode : int64_t {
        Overwrite = 0,
        AddBlend = 1,
        Private = 2,
    };
}
namespace via::io::file::FileInfo {
    enum class ATTR : int64_t {
        DIRECTORY = 1,
        ARCHIVABLE = 2,
        HIDDEN = 4,
        NORMAL = 8,
        READONLY = 16,
        SYSTEM = 32,
    };
}
namespace via::motion::Chain {
    enum class WindDirection : int64_t {
        Global = 0,
        Local = 1,
    };
}
namespace app::ropeway::gamemastering::MainFlowManager {
    enum class SoundState : int64_t {
        TITLE = 0,
        CHARACTER_SELECT = 1,
        DIFFICULTY_SELECT = 2,
        CAMERA_SELECT = 3,
        PAUSE = 4,
        MAP = 5,
        INVENTORY = 6,
        SAVE = 7,
        GAMEOVER = 8,
        IN_GAME = 9,
        LOADING = 10,
        STAFFROLL = 11,
        RESULT = 12,
        RESULT_EXTRA = 13,
        BONUS_FIGURE = 14,
        SHOP = 15,
        BACKPACK = 16,
    };
}
namespace app::ropeway::enemy::em6200::MotionPattern {
    enum class LadderUpEnd : int64_t {
        Arm_L = 0,
        Arm_R = 1,
        Arm_L_Under = 2,
        Arm_R_Under = 3,
    };
}
namespace via::gui {
    enum class CommonState : int64_t {
        Default = 0,
        Focus = 1,
        Select = 2,
        Unfocus = 3,
        Disable = 4,
        DisableFocus = 5,
        DisableSelect = 6,
        DisableUnfocus = 7,
        FadeIN_ = 8,
        FadeOut = 9,
        Decide = 10,
        PlusInput = 11,
        MinusInput = 12,
        Max = 13,
    };
}
namespace via::storage::saveService {
    enum class SaveState : int64_t {
        None = 0,
        IDLE = 1,
        SaveDialogStart = 2,
        LoadDialogStart = 3,
        RemoveDialogStart = 4,
        SaveDialogRun = 5,
        LoadDialogRun = 6,
        RemoveDialogRun = 7,
        DialogIDLE = 8,
        SaveStart = 9,
        LoadStart = 10,
        RemoveStart = 11,
        SaveRun = 12,
        LoadRun = 13,
        RemoveRun = 14,
        ErrorDialog = 15,
        Max = 16,
    };
}
namespace app::ropeway::gamemastering::GlobalUserDataManager {
    enum class WriteProtectState : int64_t {
        NONE = 0,
        SAVE_PROTECT = 1,
        LOAD_PROTECT = 2,
    };
}
namespace app::ropeway::enemy::em7100::Em7100Think {
    enum class ContainerHitDeadType : int64_t {
        ContainerHitCountType = 0,
        KneelDownType = 1,
        ContainerHitCounOrKneelDownType = 2,
        ContainerHitHpRateType = 3,
    };
}
namespace via::render::RenderTargetOperator {
    enum class StreamState : int64_t {
        NotRequested = 0,
        Failed = 1,
        Requested = 2,
        Processing = 3,
        Finished = 4,
    };
}
namespace app::ropeway::gamemastering::network::NetworkManager {
    enum class Phase : int64_t {
        None = 0,
        Entry = 1,
        StartContextWait = 2,
        StartedContext = 3,
        FinalizeContext = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickPressed {
    enum class PlayerType : int64_t {
        MALE = 0,
        FEMALE = 1,
        CHILD = 2,
        MAX = 3,
    };
}
namespace via::motion::IkLeg {
    enum class ToeCtrlOption : int64_t {
        None = 0,
        TwoRayCast = 1,
        ToeOnly = 2,
    };
}
namespace via::effect::detail {
    enum class PtClipAttribute : int64_t {
        BindToLife = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickRogueCharaSelect {
    enum class ArgNo : int64_t {
        Open = 0,
        Wait = 1,
    };
}
namespace via::hid {
    enum class DeviceType : int64_t {
        Unknown = 0,
        NULL_ = 1,
        GeneralGamePad = 2,
        SpecialGamePad = 3,
        Dualshock4 = 4,
        XboxOneWirelessController = 5,
        Npad = 6,
        TouchScreen = 7,
        Keyboard = 8,
        Mouse = 9,
        PlayStationCamera = 10,
        Kinect = 11,
        Morpheus = 12,
        VirtualKeyboard = 13,
    };
}
namespace app::ropeway::Em6100Think {
    enum class STATE : int64_t {
        Swim = 0,
        Jump = 1,
        Attack = 2,
        Dead = 3,
        Turn = 4,
        Float = 5,
    };
}
namespace via::motion::ActionPlayMotion {
    enum class FrameControl : int64_t {
        Normal = 0,
        SyncBaseLayer = 1,
        PauseStartFrame = 2,
        PauseEndFrame = 3,
    };
}
namespace via::hid {
    enum class AccountPickerResults : int64_t {
        NULL_ = 0,
        Completed = 1,
        Failed = 2,
    };
}
namespace app::ropeway::Em6100Manager {
    enum class STATE : int64_t {
        None = 0,
        Init = 1,
        Move = 2,
        Delete = 3,
    };
}
namespace app::ropeway::camera::FigureCameraController {
    enum class FigureCameraUpdateState : int64_t {
        Default = 0,
        InList = 1,
        Setup = 2,
        ListToDetail = 3,
        InDetail = 4,
        DetailToList = 5,
        DetailToListWithReset = 6,
        Reset = 7,
    };
}
namespace app::ropeway::camera::userdata::PlyaerCameraForceTwirlerUserData {
    enum class TriggerType : int64_t {
        Start = 0,
        End = 1,
        Elapsed = 2,
    };
}
namespace app::ropeway::behaviortree::action::survivor::npc::NpcBtCondition_CheckJackAction {
    enum class CheckTypeKind : int64_t {
        IsStarted = 0,
        IsWaitStart = 1,
    };
}
namespace app::ropeway::gui::TitleBackgroundScene {
    enum class TimelineStartState : int64_t {
        OPEN = 0,
        DECIDE = 1,
        BACK = 2,
    };
}
namespace via::motion::IkDamageAction {
    enum class CalcuCenterOffset : int64_t {
        ImpactState = 0,
        MaxState = 1,
    };
}
namespace via::dialog::VrServiceDialog {
    enum class Mode : int64_t {
        Positioning = 0,
        TipsForEnvironment = 1,
    };
}
namespace via::motion::tree::RoundingNode {
    enum class Mode : int64_t {
        Round = 0,
        Floor = 1,
        Ceil = 2,
    };
}
namespace via::nnfc::nfp {
    enum class NfpError : int64_t {
        None = 0,
        NotSupport = 1,
        InvalidFormat = 2,
        WirelessOff = 3,
        TagLost = 4,
        TagLostOnFlush = 5,
        NoAppData = 6,
        MismatchApp = 7,
        NeedRegister = 8,
        NeedRestore = 9,
        NeedFormat = 10,
        DifferentTag = 61440,
        MismatchUser = 61441,
        Unknown = 65535,
    };
}
namespace app::ropeway::enemy::em6200 {
    enum class SetMode : int64_t {
        Normal = 0,
        Aida = 1,
        Parking = 2,
        Sherry = 3,
        Laboratory = 4,
        UnderLaboratory = 5,
        FourthSurvivor = 6,
    };
}
namespace app::ropeway::gui::PauseBehavior {
    enum class Mode : int64_t {
        ROOT = 0,
        RETURN_TO_GAME = 1,
        LOAD = 2,
        LOAD_WAIT = 3,
        RECORD = 4,
        LMODE_RECORD = 5,
        CLEAR_STATE = 6,
        COSTUME = 7,
        COSTUME_WAIT = 8,
        OPTIONS = 9,
        OPTIONS_WAIT = 10,
        QUIT = 11,
        QUIT_WAIT = 12,
        WAIT_CLOSE = 13,
    };
}
namespace app::ropeway::gui::RogueInGameShopBehavior {
    enum class Mode : int64_t {
        Disable = 0,
        DisableBlackOut = 1,
        EnableDisplay = 2,
        EnableDisplayNoMove = 3,
        EnableDisplayAndControl = 4,
        EnableDisplayAndControlFree = 5,
    };
}
namespace via::render::Wrinkle {
    enum class CalcMode : int64_t {
        Maximum = 0,
        Average = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickKeyDevice4SeizedLockersControl {
    enum class PartsId : int64_t {
        KEY_0 = 100,
        KEY_1 = 101,
        KEY_2 = 102,
        KEY_3 = 103,
        KEY_4 = 104,
        KEY_5 = 105,
        KEY_6 = 106,
        KEY_7 = 107,
        KEY_8 = 108,
        KEY_9 = 109,
        KEY_BACK = 110,
        KEY_ENTER = 111,
        EMPTY_2 = 122,
        EMPTY_3 = 123,
        BLANK_2 = 124,
        BLANK_3 = 125,
    };
}
namespace app::ropeway::gamemastering::PauseFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        UPDATE = 1,
        RETURN = 2,
        LOAD = 3,
        TO_TITLE = 4,
        FINALIZE = 5,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class RequestSetColliderResouceIndex : int64_t {
        Common = 0,
        Skeleton = 1,
    };
}
namespace via::gui {
    enum class BlendType : int64_t {
        Alpha = 0,
        Add = 1,
        Disable = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class BreakLevel : int64_t {
        NONE = 0,
        LEVEL_1 = 1,
        LEVEL_2 = 2,
        LEVEL_3 = 3,
        LEVEL_4 = 4,
    };
}
namespace app::ropeway::gimmick::option::GimmickCameraSettings {
    enum class AnglePriority : int64_t {
        HIGHEST = 0,
        HIGH = 1,
        MIDDLE = 2,
        LOW = 3,
        LOWEST = 4,
        EXCLUDED = 5,
    };
}
namespace app::ropeway::Em6100Think {
    enum class TEMPER : int64_t {
        Mild = 0,
        Gentle = 1,
        Hasty = 2,
        NUM = 3,
    };
}
namespace app::ropeway::behaviortree::action::RandomNodeTimer {
    enum class TimeUnitType : int64_t {
        Frame = 0,
        Second = 1,
    };
}
namespace via::gui::detail {
    enum class PageAlignmentH : int64_t {
        Left = 0,
        Center = 1,
        Right = 2,
    };
}
namespace app::ropeway::gamemastering::SaveDataManager {
    enum class SaveDataNumber : int64_t {
        SYSTEM_DATA_BEGIN_ = 0xFFFFFFFF,
        SYSTEM_DATA_COUNT = 1,
        SYSTEM_DATA_END = 0xFFFFFFFF,
        SCENARIO_DATA_BEGIN_ = 0,
        SCENARIO_DATA_COUNT = 21,
        SCENARIO_DATA_END = 20,
        ROGUE_DATA_BEGIN_ = 21,
        ROGUE_DATA_COUNT = 11,
        ROGUE_DATA_END = 31,
        DEBUG_DATA_BEGIN_ = 32,
        DEBUG_DATA_COUNT = 200,
        DEBUG_DATA_END = 231,
        MAX = 232,
    };
}
namespace via::render {
    enum class InstanceKeyState : int64_t {
        Invalid = 0,
        Complete = 1,
    };
}
namespace app::ropeway::gimmick::option::GimmickOptionEnemyTrapSettings {
    enum class AttackColShape_Enum : int64_t {
        BOX = 0,
        SPHERE = 1,
        CAPSULE = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickMovingRoom {
    enum class RoutineType : int64_t {
        WAIT = 0,
        START = 1,
        LOOP = 2,
        GOAL = 3,
        SLEEP = 4,
    };
}
namespace via::autoplay::action::AutoButton {
    enum class Activation : int64_t {
        Together = 0,
        Sequence = 1,
    };
}
namespace app::ropeway::leveldesign::ScenarioActivateController {
    enum class Judge : int64_t {
        Blank = 0,
        Off = 1,
        On = 2,
    };
}
namespace app::ropeway::PrefabExtension {
    enum class InstantiateRequestResult : int64_t {
        Accepted = 0,
        RefusedByPrefabUnloaded = 1,
        RefusedByIlligalArgument = 2,
        RefusedByInvalidPrefab = 3,
        RefusedByUnknownReason = 4,
    };
}
namespace via::gui::GlowManager {
    enum class BlurFilterType : int64_t {
        BlurMipMapHorizontal = 0,
        BlurMipMapVertical = 1,
        BlurFinal = 2,
    };
}
namespace via::gui::MessageTag {
    enum class Type : int64_t {
        Unknown = 0,
        Page = 1,
        Line = 2,
        Size = 3,
        Font = 4,
        Color = 5,
        Char = 6,
        Space = 7,
        Wordwrap = 8,
        Center = 9,
        Left = 10,
        Right = 11,
        Top = 12,
        Bottom = 13,
        Time = 14,
        Ruby = 15,
        RubyRB = 16,
        RubyRT = 17,
        Draw = 18,
        GlowDraw = 19,
        ShadowDraw = 20,
        Sprite = 21,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class StandStatus : int64_t {
        STAND = 0,
        LOWSTANCE = 1,
        LAY = 2,
    };
}
namespace via::autoplay::action::AutoPlayTimer {
    enum class Unit : int64_t {
        Second = 0,
        Frame = 1,
    };
}
namespace via::timeline {
    enum class LoadState : int64_t {
        Initialize = 0,
        Loading = 1,
        Reload = 2,
        Normal = 3,
        Empty = 4,
    };
}
namespace app::ropeway::SurvivorAssignManager {
    enum class CastingReason : int64_t {
        None = 0,
        InitialSet = 1,
        Event = 2,
        Level = 4,
        Debug = 8,
    };
}
namespace via::physics::FilterInfoData {
    enum class Attribute : int64_t {
        MaskNAND = 0,
        Max = 1,
    };
}
namespace app::ropeway::camera::fsmv2::action::PlayerCameraForceTwirlRequest::TwirlerSetting {
    enum class DirectionType : int64_t {
        Owner = 0,
        Player = 1,
    };
}
namespace via::motion::tree::TransitionNode {
    enum class Mode : int64_t {
        Default = 0,
        Sync = 1,
    };
}
namespace via::render::PrimitiveFluid2D::Attribute {
    enum class ParticleType : int64_t {
        Billboard = 0,
        Polygon = 1,
        Ribbon = 2,
    };
}
namespace app::ropeway::gui::DefenseItemIconBehavior {
    enum class ButtonType : int64_t {
        Invalid = 0,
        KNIFE = 1,
        STUN_GUN = 2,
        GRENADE = 3,
        F_GRENADE = 4,
    };
}
namespace app::ropeway::SparkGimmickAttackUserDataToEnemy {
    enum class SparkGimmickType : int64_t {
        Normal = 0,
        Special = 1,
        Notice = 2,
    };
}
namespace app::ropeway::gui::NewInventoryDetailBehavior {
    enum class ItemModelState : int64_t {
        Ready = 0,
        Wait = 1,
        Success = 2,
        Failure = 3,
    };
}
namespace via::render {
    enum class PCTargetAPI : int64_t {
        DirectX11 = 0,
        DirectX12 = 1,
    };
}
namespace System::Reflection {
    enum class EventAttributes : int64_t {
        None = 0,
        SpecialName = 512,
        RTSpecialName = 1024,
        ReservedMask = 1024,
    };
}
namespace app::ropeway::enemy::em4000::tracks::Em4000DamageMotTypeTrack {
    enum class MotType : int64_t {
        DEFAULT = 0,
        DMG_BARRICADE = 1,
        DMG_KENNEL = 2,
        DMG_DEAD_OK = 3,
    };
}
namespace via::navigation::MapAccessor {
    enum class SearchResult : int64_t {
        Found = 0,
        Wait = 1,
        None = 2,
    };
}
namespace via::render::RenderConfig {
    enum class AntiAliasingType : int64_t {
        FXAA = 0,
        TAA = 1,
        FXAA_TAA = 2,
        SMAA = 3,
        NONE = 4,
    };
}
namespace via::render {
    enum class MapType : int64_t {
        WriteDiscard = 0,
        Read = 1,
    };
}
namespace via::autoplay::action::AutoNavigationPoint {
    enum class ReachDistanceType : int64_t {
        Long = 0,
        Normal = 1,
    };
}
namespace via::render {
    enum class AsyncPipe : int64_t {
        Disable = 0,
        Compute = 1,
        Copy = 2,
        ComputeJustNow = 3,
        CopyJustNow = 4,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class ColliderSkipId : int64_t {
        None = 0,
        Press = 1,
        TouchAppeal = 2,
        GimmickSensor = 4,
        EffectSensor = 8,
        SoundSensor = 16,
        SoundMarkerEnemyAttack = 32,
        DamageUpper = 1024,
        DamageMiddle = 2048,
        DamageLower = 4096,
        All = 0xFFFFFFFF,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0200_MONTAGE_PARTS_PANTS : int64_t {
        NONE = 0,
        PANTS00 = 1,
        PANTS01 = 2,
        PANTS02 = 3,
        PANTS04 = 4,
        PANTS70 = 5,
    };
}
namespace via::navigation::NavigationWaypoint {
    enum class EndNodeUsage : int64_t {
        Trace = 0,
        Skip = 1,
    };
}
namespace via::hid::virtualKeyboard::nsw {
    enum class InitialCursorPos : int64_t {
        First = 0,
        Last = 1,
    };
}
namespace app::ropeway::enemy::em8500::param::Em8500ContextParameter {
    enum class Em8500FaceMontageName : int64_t {
        POISONFACE00 = 0,
        POISONFACE01 = 1,
        POISONFACE02 = 2,
        POISONFACE03 = 3,
    };
}
namespace app::ropeway::enemy::em6100::fsmv2::action::Em6100FsmAction_Wait {
    enum class SET_TYPE : int64_t {
        Born = 0,
        Attack = 1,
    };
}
namespace via::movie::MovieManager {
    enum class GCTarget : int64_t {
        RAM = 0,
        VRAM = 1,
    };
}
namespace System::Globalization {
    enum class UnicodeCategory : int64_t {
        UppercaseLetter = 0,
        LowercaseLetter = 1,
        TitlecaseLetter = 2,
        ModifierLetter = 3,
        OtherLetter = 4,
        NonSpacingMark = 5,
        SpacingCombiningMark = 6,
        EnclosingMark = 7,
        DecimalDigitNumber = 8,
        LetterNumber = 9,
        OtherNumber = 10,
        SpaceSeparator = 11,
        LineSeparator = 12,
        ParagraphSeparator = 13,
        Control = 14,
        Format = 15,
        Surrogate = 16,
        PrivateUse = 17,
        ConnectorPunctuation = 18,
        DashPunctuation = 19,
        OpenPunctuation = 20,
        ClosePunctuation = 21,
        InitialQuotePunctuation = 22,
        FinalQuotePunctuation = 23,
        OtherPunctuation = 24,
        MathSymbol = 25,
        CurrencySymbol = 26,
        ModifierSymbol = 27,
        OtherSymbol = 28,
        OtherNotAssigned = 29,
    };
}
namespace via::nnfc {
    enum class NnfcMode : int64_t {
        Invalid = 0,
        NFP = 1,
    };
}
namespace app::ropeway::effect::script::WarheadStandardEffectPlayer {
    enum class RequestEffectType : int64_t {
        Default = 0,
        Wall = 1,
        Aerial = 2,
        Enemy = 3,
    };
}
namespace app::ropeway::gui::GUIMaster::GuiInput {
    enum class LockDir : int64_t {
        NONE = 0,
        LEFT = 1,
        RIGHT = 2,
        UP = 4,
        DOWN = 8,
    };
}
namespace via::autoplay::action::AutoButton {
    enum class Unit : int64_t {
        Seconds = 0,
        Frames = 1,
    };
}
namespace app::ropeway::enemy::em6000 {
    enum class RequestSetID : int64_t {
        INVALID = 0xFFFFFFFF,
        PRESS = 0,
        DAMAGE = 1,
        SENSOR_TOUCH = 2,
        ATTACK_PUNCH = 3,
        ATTACK_DASH_PUNCH = 4,
        ATTACK_SWIMMING = 5,
        ATTACK_STANDUP = 6,
        ATTACK_BACK_SLAP = 7,
        ATTACK_GRAPPLE = 8,
        ATTACK_RIGHT_HOOK = 9,
        ATTACK_PUNCH_COMBO = 10,
        ATTACK_HIDE = 11,
        ATTACK_WAVE = 12,
        CAMERA = 13,
        DAMAGE_PUS = 14,
        DAMAGE_OVIDUCT = 15,
        ATTACK_VOMIT = 16,
        ATTACK_VOMIT_CANSEL = 17,
        ATTACK_UPPER = 18,
        DAMAGE_EYE = 19,
        AIM_TARGET = 20,
    };
}
namespace via::motion::MotionManager {
    enum class CallUpdate : int64_t {
        ComponentCollection = 0,
        Structure = 1,
        Frame = 2,
        FrameAsync = 3,
        Motion = 4,
        ConstraintBegIN_ = 5,
        ConstraintEnd = 6,
        Expression = 7,
        DebugDraw = 8,
        ReloadCheck = 9,
    };
}
namespace app::ropeway::enemy::em6100 {
    enum class RankParamHash : int64_t {
        MOVE_SPEED_SCALE = 225628402,
    };
}
namespace app::ropeway::enemy::em6300 {
    enum class AttackPatternType : int64_t {
        Rush = 0,
    };
}
namespace app::ropeway::gui::PurposeBehavior {
    enum class State : int64_t {
        INVALID = 0,
        ONE_TIME = 1,
        INVENTORY = 2,
    };
}
namespace app::ropeway::gui::NewInventoryDetailBehavior {
    enum class Mode : int64_t {
        Invalid = 0,
        Detail = 1,
        PickUpNormal = 2,
        PickUpFirst = 3,
        PickUpFirstCombination = 4,
        PickUpFirstCombinationDemo = 5,
        PickUpMap = 6,
    };
}
namespace via::gui::Memo {
    enum class MemoRotationType : int64_t {
        Billboard = 0,
        TransformAndOffset = 1,
        OffsetOnly = 2,
    };
}
namespace app::ropeway::CatalogManager::CatalogContext {
    enum class CatalogType : int64_t {
        Invalid = 0,
        First = 1,
        Additional = 2,
        External = 3,
    };
}
namespace via::storage {
    enum class CompressionLevel : int64_t {
        Default = 0,
        Maximum = 1,
    };
}
namespace app::ropeway::enemy::em7300::fsmv2::condition::TargetDirection {
    enum class CompareType : int64_t {
        Equal = 0,
        NotEqual = 1,
        Less = 2,
        LessEq = 3,
        Greater = 4,
        GreaterEq = 5,
    };
}
namespace via::clr {
    enum class ExceptionTranslationFlag : int64_t {
        None = 0,
        InvalidCast = 1,
        IndexOutOfRange = 2,
        NullReference = 4,
        ArgumentOutOfRange = 8,
        ArgumentNULL_ = 16,
        InvalidOperation = 32,
        StackOverflow = 64,
        Argument = 128,
        Format = 256,
        NotImplemented = 512,
        NotSupported = 1024,
        Any = 65535,
    };
}
namespace app::ropeway::gamemastering::InventoryManager {
    enum class WeaponShortcut : int64_t {
        UP = 0,
        DOWN = 1,
        RIGHT = 2,
        LEFT = 3,
        MAX = 4,
        INVALID = 5,
    };
}
namespace via::os {
    enum class FileError : int64_t {
        None = 0,
        NotFound = 1,
        AccessDenied = 2,
        TooManyOpenFiles = 3,
        InvalidOperation = 4,
        Critical = 5,
    };
}
namespace app::ropeway::gui::NewInventoryBehavior {
    enum class State : int64_t {
        NoMove = 0,
        Enter = 1,
        Capture = 2,
        Ready = 3,
        Ready2 = 4,
        Move = 5,
        Leave = 6,
        Finish = 7,
    };
}
namespace JsonParser {
    enum class Phase : int64_t {
        Idle = 0,
        Name = 1,
        Value = 2,
    };
}
namespace app::ropeway::LookAt {
    enum class TargetModeEnum : int64_t {
        Position = 0,
        GameObject = 1,
        Joint = 2,
    };
}
namespace via::os {
    enum class ConditionVariableError : int64_t {
        None = 0,
        Failed = 1,
        TimedOut = 2,
    };
}
namespace app::ropeway::gamemastering::RogueStorageManager {
    enum class QueryResultTypeAdd : int64_t {
        InValid = 0xFFFFFFFF,
        FullCombinedToSingleItem = 0,
        FullCombinedToMultiItems = 1,
        CombinedAndAddToBlankSlot = 2,
        AddToBlankSlot = 3,
        PartlyCombinedButCouldntAddToSlot = 4,
        CouldntAddToSlot = 5,
    };
}
namespace app::ropeway::gui::NewInventoryDetailBehavior {
    enum class ItemDetailState : int64_t {
        NoMove = 0,
        Wait = 1,
        Start = 2,
        Enter = 3,
        Move = 4,
        WaitDefault = 5,
        Leave = 6,
        Angle = 7,
        PlayMotion = 8,
        WaitMotion = 9,
        Action = 10,
        WaitText = 11,
        Change = 12,
        ChangeName = 13,
        ChangeOut = 14,
        ChangeWait = 15,
        ChangeIN_ = 16,
        ChangeEnd = 17,
        PuzzleStart = 18,
        PuzzleWait = 19,
        PuzzleDemo = 20,
        PuzzleEnd = 21,
        Gimmick = 22,
        GetKeyItem_IN_ = 23,
        GetKeyItem_Move = 24,
        GetKeyItem_Update = 25,
        GetKeyItem_Wait = 26,
        CombineStart = 27,
        CombinePlayMotion = 28,
        CombineWaitMotion = 29,
        CombineChangeOut = 30,
        CombineChangeWait = 31,
        CombineChangeIN_ = 32,
        CombineChangeEnd = 33,
    };
}
namespace via::gui {
    enum class ResolutionAdjustAnchor : int64_t {
        LeftTop = 0,
        LeftCenter = 1,
        LeftBottom = 2,
        CenterTop = 3,
        CenterCenter = 4,
        CenterBottom = 5,
        RightTop = 6,
        RightCenter = 7,
        RightBottom = 8,
        Max = 9,
    };
}
namespace via::render::layer::ShadowCast {
    enum class ShadowCastSegment : int64_t {
        StaticShadow = 0,
        ShadowClear = 1,
        ShadowSolid = 4,
        ShadowTwoSide = 5,
        ShadowTwoSideAlphaTest = 6,
        ShadowAlphaTest = 7,
        ShadowDitherCoverage = 8,
        StaticShadowSolid = 4,
        StaticShadowTwoSide = 5,
        StaticShadowTwoSideAlphaTest = 6,
        StaticShadowAlphaTest = 7,
        CacheCopy = 16,
        DynamicShadow = 32,
        DynamicShadowSolid = 36,
        DynamicShadowTwoSide = 37,
        DynamicShadowTwoSideAlphaTest = 38,
        DynamicShadowAlphaTest = 39,
        DynamicShadowDitherCoverage = 40,
        Finalize = 48,
    };
}
namespace via::motion::GpuMotion {
    enum class PlayState : int64_t {
        Play = 0,
        Pause = 1,
        Stop = 2,
        Max = 3,
    };
}
namespace app::ropeway::gui::GimmickNumberLockGuiBehavior {
    enum class State : int64_t {
        NoMove = 0,
        Wait = 1,
        Satisfy = 2,
        Incorrect = 3,
    };
}
namespace via::hid::VrTracker {
    enum class OrientationType : int64_t {
        Absolute = 0,
        Relative = 1,
    };
}
namespace via::effect::detail {
    enum class PlayOrder : int64_t {
        Forward = 0,
        Reverse = 1,
        RandomReverse = 2,
    };
}
namespace app::ropeway::gui::GimmickNumberLockGuiBehavior {
    enum class KeyPositionEnum : int64_t {
        Invalid = 0xFFFFFFFF,
        PosKey7 = 0,
        PosKey8 = 1,
        PosKey9 = 2,
        PosKey4 = 3,
        PosKey5 = 4,
        PosKey6 = 5,
        PosKey1 = 6,
        PosKey2 = 7,
        PosKey3 = 8,
        PosKey0L = 9,
        PosKey0R = 10,
        PosKeyBack = 11,
        PosKeyEnterL = 12,
        PosKeyEnterC = 13,
        PosKeyEnterR = 14,
    };
}
namespace app::ropeway::enemy::em3000::MotionPattern {
    enum class Burst : int64_t {
        FromF = 0,
        FromB = 1,
        FromL = 2,
        FromR = 3,
    };
}
namespace via::motion::script::MotionExtraData {
    enum class ContactStatus : int64_t {
        None = 0,
        LeftLegContact = 1,
        RightLegContact = 2,
        LeftLegStop = 4,
        RightLegStop = 8,
        LeftLegFix = 5,
        RightLegFix = 10,
        BothLegContact = 3,
        BothLegFix = 15,
        LeftLegStep = 16,
        RightLegStep = 32,
        LeftToeStop = 64,
        RightToeStop = 128,
        LeftToeFix = 65,
        RightToeFix = 130,
        BothToeFix = 195,
        LegAll = 255,
        LeftHandContact = 256,
        RightHandContact = 512,
        LeftHandStop = 1024,
        RightHandStop = 2048,
        LeftHandFix = 1280,
        RightHandFix = 2560,
        BothHandContact = 768,
        BothHandFix = 3840,
        LeftHandStep = 4096,
        RightHandStep = 8192,
        LeftHandTipStop = 16384,
        RightHandTipStop = 32768,
        LeftHandTipFix = 16640,
        RightHandTipFix = 33280,
        BothHandTipFix = 49920,
    };
}
namespace app::ropeway::gamemastering::FsmStateManager {
    enum class UpdateResult : int64_t {
        Continued = 0,
        Completed = 1,
        Failed = 2,
    };
}
namespace app::ropeway::gimmick::option::GimmickJumpDownAIMapEffectorShape {
    enum class PointType : int64_t {
        LOW = 0,
        HIGH = 1,
    };
}
namespace via::motion::detail::IkFourLegSolver {
    enum class SolverType : int64_t {
        SOLVE_2BONE = 0,
        SOLVE_3BONE = 1,
    };
}
namespace via::AnimationCurve {
    enum class Wrap : int64_t {
        Once = 0,
        Loop = 1,
        Loop_Always = 2,
    };
}
namespace app::ropeway::navigation::Definition {
    enum class Attr1 : int64_t {
        Door = 1,
        Ladder = 2,
        Box = 4,
        G2Jump = 8,
        Step = 16,
        Ladder_Upper = 32,
        Ladder_Lower = 64,
        Edge_Upper = 128,
        Edge_Lower = 256,
        Edge = 512,
        Detour00 = 1024,
        Detour01 = 2048,
        Licker = 4096,
        Divide = 8192,
    };
}
namespace app::ropeway::ChoreographPlayer::ChoreographTimeline {
    enum class GimmickEventEnd : int64_t {
        NONE = 0,
        FROM_BODY = 1,
        FROM_CTRL = 2,
    };
}
namespace app::ropeway::enemy::em7100 {
    enum class SetType : int64_t {
        SetType_Default = 0,
        SetType_G3 = 1,
        SetType_WasteWater = 2,
        SetType_Rogue = 3,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::SetMotionBlendRate {
    enum class SET_METHOD_TYPE : int64_t {
        START = 0,
        UPDATE = 1,
    };
}
namespace app::ropeway::gamemastering::MapParts {
    enum class ID : int64_t {
        Invalid = 0,
        st0_101_0 = 1,
        st0_102_0 = 2,
        st1_101_0 = 3,
        st1_102_0 = 4,
        st1_103_0 = 5,
        st1_104_0 = 6,
        st1_105_0 = 7,
        st1_106_0 = 8,
        st1_107_0 = 9,
        st1_108_0 = 10,
        st1_109_0 = 11,
        st1_110_0 = 12,
        st1_111_0 = 13,
        st1_201_0 = 14,
        st1_202_0 = 15,
        st1_203_0 = 16,
        st1_204_0 = 17,
        st1_205_0 = 18,
        st1_206_0 = 19,
        st1_207_0 = 20,
        st1_208_0 = 21,
        st1_209_0 = 22,
        st1_210_0 = 23,
        st1_211_0 = 24,
        st2_101_0 = 25,
        st2_102_0 = 26,
        st2_103_0 = 27,
        st2_201_0 = 28,
        st2_202_0 = 29,
        st2_203_0 = 30,
        st2_204_0 = 31,
        st2_205_0 = 32,
        st2_206_0 = 33,
        st2_207_0 = 34,
        st2_208_0 = 35,
        st2_209_0 = 36,
        st2_210_0 = 37,
        st2_211_0 = 38,
        st2_212_0 = 39,
        st2_213_0 = 40,
        st2_214_0 = 41,
        st2_215_0 = 42,
        st2_216_0 = 43,
        st2_301_0 = 44,
        st2_302_0 = 45,
        st2_303_0 = 46,
        st2_304_0 = 47,
        st2_305_0 = 48,
        st2_306_0 = 49,
        st2_307_0 = 50,
        st2_308_0 = 51,
        st2_309_0 = 52,
        st2_401_0 = 53,
        st2_401_1 = 54,
        st2_402_0 = 55,
        st2_403_0 = 56,
        st2_404_0 = 57,
        st2_405_0 = 58,
        st2_406_0 = 59,
        st2_407_0 = 60,
        st2_408_0 = 61,
        st2_501_0 = 62,
        st2_502_0 = 63,
        st2_502_0b = 64,
        st2_502_0c = 65,
        st2_503_0 = 66,
        st2_504_0 = 67,
        st2_505_0 = 68,
        st2_506_0 = 69,
        st2_507_0 = 70,
        st2_508_0 = 71,
        st2_601_0 = 72,
        st2_602_0 = 73,
        st2_603_0 = 74,
        st2_604_0 = 75,
        st2_605_0 = 76,
        st2_606_0 = 77,
        st2_607_0 = 78,
        st2_608_0 = 79,
        st3_101_0 = 80,
        st3_102_0 = 81,
        st3_103_0 = 82,
        st3_104_0 = 83,
        st3_105_0 = 84,
        st3_106_0 = 85,
        st3_107_0 = 86,
        st3_108_0 = 87,
        st3_109_0 = 88,
        st3_110_0 = 89,
        st3_111_0 = 90,
        st3_112_0 = 91,
        st3_113_0 = 92,
        st3_114_0 = 93,
        st3_201_0 = 94,
        st3_202_0 = 95,
        st3_203_0 = 96,
        st3_204_0 = 97,
        st3_205_0 = 98,
        st3_206_0 = 99,
        st3_207_0 = 100,
        st3_208_0 = 101,
        st3_209_0 = 102,
        st3_210_0 = 103,
        st3_301_0 = 104,
        st3_302_0 = 105,
        st3_303_0 = 106,
        st3_304_0 = 107,
        st3_305_0 = 108,
        st3_306_0 = 109,
        st3_307_0 = 110,
        st3_308_0 = 111,
        st4_101_0 = 112,
        st4_102_0 = 113,
        st4_103_0 = 114,
        st4_104_0 = 115,
        st4_105_0 = 116,
        st4_106_0 = 117,
        st4_207_0 = 118,
        st4_208_0 = 119,
        st4_209_0 = 120,
        st4_210_0 = 121,
        st4_211_0 = 122,
        st4_212_0 = 123,
        st4_213_0 = 124,
        st4_214_0 = 125,
        st5_101_0 = 126,
        st5_102_0 = 127,
        st5_103_0 = 128,
        st5_104_0 = 129,
        st5_105_0 = 130,
        st5_106_0 = 131,
        st5_107_0 = 132,
        st5_201_0 = 133,
        st5_202_0 = 134,
        st5_203_0 = 135,
        st5_204_0 = 136,
        st5_205_0 = 137,
        st5_206_0 = 138,
        st5_207_0 = 139,
        st5_301_0 = 140,
        st5_302_0 = 141,
        st5_303_0 = 142,
        st6_101_0 = 143,
        st6_102_0 = 144,
        st6_103_0 = 145,
        st6_104_0 = 146,
        st6_105_0 = 147,
        st6_106_0 = 148,
        st6_107_0 = 149,
        st6_201_0 = 150,
        st6_202_0 = 151,
        st6_203_0 = 152,
        st6_204_0 = 153,
        st6_205_0 = 154,
        st6_206_0 = 155,
        st6_207_0 = 156,
        st6_208_0 = 157,
        st6_209_0 = 158,
        st6_210_0 = 159,
        st6_211_0 = 160,
        st6_212_0 = 161,
        st7_101_0 = 162,
        st7_102_0 = 163,
        st7_103_0 = 164,
        st7_104_0 = 165,
        st7_105_0 = 166,
        st7_106_0 = 167,
        st7_107_0 = 168,
        st7_108_0 = 169,
        st7_109_0 = 170,
        st7_110_0 = 171,
        st7_111_0 = 172,
        st7_112_0 = 173,
        st8_101_0 = 174,
        st8_102_0 = 175,
        st8_103_0 = 176,
        st8_104_0 = 177,
        st8_105_0 = 178,
        st8_106_0 = 179,
        st8_107_0 = 180,
        st8_108_0 = 181,
        st8_109_0 = 182,
        st8_110_0 = 183,
        st8_111_0 = 184,
        st8_112_0 = 185,
        st8_113_0 = 186,
        st8_114_0 = 187,
        st8_115_0 = 188,
        st8_201_0 = 189,
        st8_202_0 = 190,
        st8_203_0 = 191,
        st8_301_0 = 192,
        st8_302_0 = 193,
        st8_303_0 = 194,
        st8_304_0 = 195,
        st8_305_0 = 196,
        st8_306_0 = 197,
        st8_307_0 = 198,
        st8_308_0 = 199,
        st8_309_0 = 200,
        st8_310_0 = 201,
        st8_311_0 = 202,
        st8_401_0 = 203,
        st8_402_0 = 204,
        st8_403_0 = 205,
        st8_404_0 = 206,
        st8_405_0 = 207,
        st8_406_0 = 208,
        st8_407_0 = 209,
        st9_101_0 = 210,
        st9_102_0 = 211,
        st9_103_0 = 212,
        st9_201_0 = 213,
        st9_202_0 = 214,
        st9_203_0 = 215,
        st9_301_0 = 216,
        st1_301_0 = 217,
        st1_401_0 = 218,
        st4_201_0 = 219,
        st4_202_0 = 220,
        st4_203_0 = 221,
        st4_204_0 = 222,
        st4_205_0 = 223,
        st4_206_0 = 224,
        st4_215_0 = 225,
        st4_216_0 = 226,
        st4_301_0 = 227,
        st4_302_0 = 228,
        st4_303_0 = 229,
        st4_304_0 = 230,
        st4_305_0 = 231,
        st4_306_0 = 232,
        st4_307_0 = 233,
        st4_308_0 = 234,
        st4_309_0 = 235,
        st4_310_0 = 236,
        st4_311_0 = 237,
        st4_312_0 = 238,
        st4_313_0 = 239,
        st4_314_0 = 240,
        st4_401_0 = 241,
        st4_401_1 = 242,
        st4_402_0 = 243,
        st4_403_0 = 244,
        st4_404_0 = 245,
        st4_405_0 = 246,
        st4_406_0 = 247,
        st4_407_0 = 248,
        st4_408_0 = 249,
        st4_409_0 = 250,
        st4_410_0 = 251,
        st4_411_0 = 252,
        st4_412_0 = 253,
        st4_501_0 = 254,
        st4_502_0 = 255,
        st4_503_0 = 256,
        st4_504_0 = 257,
        st4_505_0 = 258,
        st4_506_0 = 259,
        st4_507_0 = 260,
        st4_508_0 = 261,
        st4_601_0 = 262,
        st4_602_0 = 263,
        st4_603_0 = 264,
        st4_604_0 = 265,
        st4_605_0 = 266,
        st4_606_0 = 267,
        st4_607_0 = 268,
        st4_608_0 = 269,
        st4_609_0 = 270,
        st4_610_0 = 271,
        st4_701_0 = 272,
        st4_702_0 = 273,
        st4_703_0 = 274,
        st4_704_0 = 275,
        st4_705_0 = 276,
        st4_708_0 = 277,
        st4_709_0 = 278,
        st4_710_0 = 279,
        st4_711_0 = 280,
        st4_712_0 = 281,
        st4_714_0 = 282,
        st1_501_0 = 283,
        st1_502_0 = 284,
        st1_503_0 = 285,
        st1_504_0 = 286,
        st1_505_0 = 287,
        st1_506_0 = 288,
        st4_650_0 = 289,
        st3_401_0 = 290,
        st3_402_0 = 291,
        st3_403_0 = 292,
        st3_404_0 = 293,
        st3_405_0 = 294,
        st3_406_0 = 295,
        st3_407_0 = 296,
        st3_408_0 = 297,
        st3_409_0 = 298,
        st3_410_0 = 299,
        st8_501_0 = 300,
        st8_601_0 = 301,
        st8_602_0 = 302,
        st8_603_0 = 303,
        st8_604_0 = 304,
        st8_605_0 = 305,
        st8_606_0 = 306,
        st8_607_0 = 307,
        st8_608_0 = 308,
        st8_609_0 = 309,
        st3_600_0 = 310,
        st3_601_0 = 311,
        st3_602_0 = 312,
        st3_603_0 = 313,
        st3_610_0 = 314,
        st3_611_0 = 315,
        st3_612_0 = 316,
        st3_613_0 = 317,
        st3_614_0 = 318,
        st3_615_0 = 319,
        st3_616_0 = 320,
        st3_617_0 = 321,
        st3_620_0 = 322,
        st3_621_0 = 323,
        st3_622_0 = 324,
        st3_623_0 = 325,
        st3_624_0 = 326,
        st3_625_0 = 327,
        st3_626_0 = 328,
        st3_627_0 = 329,
        st3_630_0 = 330,
        st3_631_0 = 331,
        st3_632_0 = 332,
        st3_633_0 = 333,
        st3_634_0 = 334,
        st3_635_0 = 335,
        st3_636_0 = 336,
        st3_637_0 = 337,
        st3_638_0 = 338,
        st3_640_0 = 339,
        st3_641_0 = 340,
        st3_642_0 = 341,
        st3_643_0 = 342,
        st3_644_0 = 343,
        st3_650_0 = 344,
        st3_651_0 = 345,
        st3_652_0 = 346,
        st1_411_0 = 347,
        st4_715_0 = 348,
        st4_716_0 = 349,
        st4_717_0 = 350,
        st8_408_0 = 351,
        st4_750_0 = 352,
        st4_751_0 = 353,
        st4_752_0 = 354,
        st4_753_0 = 355,
        st4_754_0 = 356,
        st8_409_0 = 357,
        st8_410_0 = 358,
        st1_601_0 = 359,
        st1_602_0 = 360,
        st1_603_0 = 361,
        st1_604_0 = 362,
        st1_605_0 = 363,
        st1_606_0 = 364,
        st1_607_0 = 365,
        st1_608_0 = 366,
        st1_609_0 = 367,
        st8_650_0 = 368,
        st8_411_0 = 369,
        st5_111_0 = 370,
        st5_121_0 = 371,
        st5_131_0 = 372,
        st5_211_0 = 373,
        st5_221_0 = 374,
        st0_103_0 = 375,
        st8_660_0 = 376,
        st1_620_0 = 377,
        st1_621_0 = 378,
        st1_622_0 = 379,
        st1_623_0 = 380,
        st1_610_0 = 381,
        st1_611_0 = 382,
        st1_612_0 = 383,
        st1_613_0 = 384,
        st1_614_0 = 385,
        st5_112_0 = 386,
        st5_110_0 = 387,
        st5_113_0 = 388,
        st5_114_0 = 389,
        st5_115_0 = 390,
        st5_307_0 = 391,
        st5_308_0 = 392,
        st5_309_0 = 393,
        st5_310_0 = 394,
        st5_401_0 = 395,
        st5_402_0 = 396,
        st5_403_0 = 397,
        st5_404_0 = 398,
        st5_405_0 = 399,
        st5_406_0 = 400,
        st5_407_0 = 401,
        st5_408_0 = 402,
        st4_800_0 = 403,
        st1_615_0 = 404,
        st4_755_0 = 405,
        st1_630_0 = 406,
        st1_631_0 = 407,
        st1_632_0 = 408,
        st4_713_0 = 409,
        st5_122_0 = 410,
        st5_123_0 = 411,
        st5_124_0 = 412,
        st5_125_0 = 413,
        st5_126_0 = 414,
        st5_127_0 = 415,
        st5_128_0 = 416,
        st5_129_0 = 417,
        st5_132_0 = 418,
        st5_133_0 = 419,
        st5_134_0 = 420,
        st5_212_0 = 421,
        st5_222_0 = 422,
        st5_223_0 = 423,
        st8_651_0 = 424,
        st8_610_0 = 425,
        st8_611_0 = 426,
        st8_612_0 = 427,
        st8_613_0 = 428,
        st3_604_0 = 429,
        st3_605_0 = 430,
        st4_217_0 = 431,
        st5_122_1 = 432,
        st5_122_2 = 433,
        st4_502_0b = 435,
        st4_502_0c = 436,
        st4_504_0b = 437,
        st4_603_0b = 438,
        st4_206_0b = 439,
        st4_309_0b = 440,
        st4_603_0c = 441,
        st4_750_0b = 442,
        st3_603_0b = 443,
        st3_611_0b = 444,
        st3_613_0b = 445,
        st3_613_0c = 446,
        st3_623_0b = 447,
        st3_630_0b = 448,
        st3_630_0c = 449,
        st3_630_0d = 450,
        st4_750_0c = 451,
        st4_751_0b = 452,
        st4_701_0b = 453,
        st4_701_0c = 454,
        st4_714_0b = 455,
        st4_608_0b = 456,
        st1_601_0b = 457,
        st4_404_0b = 458,
        st3_600_0b = 459,
        st3_602_0b = 460,
        st3_627_0b = 461,
        st3_627_0c = 462,
        st3_627_0d = 463,
        st4_704_0b = 464,
        st4_750_0d = 465,
        st4_751_0c = 466,
        st4_751_0d = 467,
        st4_751_0e = 468,
        st4_751_0f = 469,
        st1_630_0b = 470,
        st1_630_0c = 471,
        st1_631_0b = 472,
        st1_632_0b = 473,
        st1_632_0c = 474,
        st1_632_0d = 475,
        st1_621_0b = 476,
        st1_623_0b = 477,
        st5_122_0b = 478,
        st5_211_0b = 479,
        st5_211_0c = 480,
        st5_203_0b = 481,
        st4_201_0b = 482,
        st4_201_0c = 483,
        st4_201_0d = 484,
        st4_201_0e = 485,
        st4_201_0f = 486,
        st3_640_0b = 487,
        st3_616_0b = 488,
        st3_616_0c = 489,
        st3_635_0b = 490,
        st3_635_0c = 491,
        st3_635_0d = 492,
        st3_635_0e = 493,
        st3_617_0b = 494,
        st3_641_0b = 495,
        st3_614_0b = 496,
        st5_211_0d = 497,
        st5_125_0b = 498,
        st3_615_0b = 499,
        st5_101_0b = 500,
        st5_133_0b = 501,
        st5_204_0b = 502,
        st5_211_0e = 503,
        st5_212_0b = 504,
        dummy070 = 505,
        dummy071 = 506,
        dummy072 = 507,
        dummy073 = 508,
        dummy074 = 509,
        dummy075 = 510,
        dummy076 = 511,
        dummy077 = 512,
        dummy078 = 513,
        dummy079 = 514,
        dummy080 = 515,
        dummy081 = 516,
        dummy082 = 517,
        dummy083 = 518,
        dummy084 = 519,
        dummy085 = 520,
        dummy086 = 521,
        dummy087 = 522,
        dummy088 = 523,
        dummy089 = 524,
        dummy090 = 525,
        dummy091 = 526,
        dummy092 = 527,
        dummy093 = 528,
        dummy094 = 529,
        dummy095 = 530,
        dummy096 = 531,
        dummy097 = 532,
        dummy098 = 533,
        dummy099 = 534,
        dummy100 = 535,
        dummy101 = 536,
        dummy102 = 537,
        dummy103 = 538,
        dummy104 = 539,
        dummy105 = 540,
        dummy106 = 541,
        dummy107 = 542,
        dummy108 = 543,
        dummy109 = 544,
        dummy110 = 545,
        dummy111 = 546,
        dummy112 = 547,
        dummy113 = 548,
        dummy114 = 549,
        dummy115 = 550,
        dummy116 = 551,
        dummy117 = 552,
        dummy118 = 553,
        dummy119 = 554,
        dummy120 = 555,
        dummy121 = 556,
        dummy122 = 557,
        dummy123 = 558,
        dummy124 = 559,
        dummy125 = 560,
        dummy126 = 561,
        dummy127 = 562,
        MAP_PARTS_NUM = 563,
    };
}
namespace app::ropeway::enemy::em3000 {
    enum class FaceDir : int64_t {
        Up = 0,
        Down = 1,
    };
}
namespace app::ropeway::fsmv2::condition::CheckCameraMode {
    enum class CameraMode : int64_t {
        NEW = 0,
        OLD = 1,
        INVALID = 2,
    };
}
namespace via::gui {
    enum class Segment : int64_t {
        Keep = 0xFFFFFFFF,
        Segment00 = 0,
        Segment01 = 1,
        Segment02 = 2,
        Segment03 = 3,
        Segment04 = 4,
        Segment05 = 5,
        Segment06 = 6,
        Segment07 = 7,
        Segment08 = 8,
        Segment09 = 9,
        Segment10 = 10,
        Segment11 = 11,
        Segment12 = 12,
        Segment13 = 13,
        Segment14 = 14,
        Segment15 = 15,
        Segment16 = 16,
        Segment17 = 17,
        Segment18 = 18,
        Segment19 = 19,
        Segment20 = 20,
        Segment21 = 21,
        Segment22 = 22,
        Segment23 = 23,
        Segment24 = 24,
        Segment25 = 25,
        Segment26 = 26,
        Segment27 = 27,
        Segment28 = 28,
        Segment29 = 29,
        Segment30 = 30,
        Segment31 = 31,
        Segment32 = 32,
        Segment33 = 33,
        Segment34 = 34,
        Segment35 = 35,
        Segment36 = 36,
        Segment37 = 37,
        Segment38 = 38,
        Segment39 = 39,
        Segment40 = 40,
        Segment41 = 41,
        Segment42 = 42,
        Segment43 = 43,
        Segment44 = 44,
        Segment45 = 45,
        Segment46 = 46,
        Segment47 = 47,
        Segment48 = 48,
        Segment49 = 49,
        Segment50 = 50,
        Segment51 = 51,
        Segment52 = 52,
        Segment53 = 53,
        Segment54 = 54,
        Segment55 = 55,
        Segment56 = 56,
        Segment57 = 57,
        Segment58 = 58,
        Segment59 = 59,
        Segment60 = 60,
    };
}
namespace app::ropeway::gui::RogueInventoryBehavior {
    enum class DetailInitialMode : int64_t {
        Enter = 0,
        Normal = 1,
        Small = 2,
    };
}
namespace app::ropeway::enemy::em9000::fsmv2::condition::TargetDirection {
    enum class CompareType : int64_t {
        Equal = 0,
        NotEqual = 1,
        Less = 2,
        LessEq = 3,
        Greater = 4,
        GreaterEq = 5,
    };
}
namespace via {
    enum class SystemServiceSummerTimeStatus : int64_t {
        Unknown = 0,
        StandardTime = 1,
        SummerTime = 2,
    };
}
namespace app::ropeway::gui::SelectControlTypeBehavior {
    enum class EndType : int64_t {
        SELECTED = 0,
        CANCELED = 1,
    };
}
namespace app::ropeway::gui::RogueFloatIconBehavior {
    enum class LostButtonType : int64_t {
        Invalid = 0,
        RUCKSACK_ICON = 1,
    };
}
namespace app::ropeway {
    enum class ViewKind : int64_t {
        Look = 0,
        Behind = 1,
        Aim = 2,
        Assist = 3,
    };
}
namespace app::ropeway::gui::GameOverBehavior {
    enum class State : int64_t {
        UNINITIALIZED = 0,
        UPDATE = 1,
        WAIT_CONTINUE = 2,
    };
}
namespace app::ropeway::effect::script::EmRainEffect {
    enum class MAT_STATE : int64_t {
        AllDry = 0,
        Drying = 1,
        Wetting = 2,
        AllWet = 3,
    };
}
namespace via::os {
    enum class SocketOption : int64_t {
        None = 0,
        TcpNoDelay = 1,
    };
}
namespace app::ropeway::gimmick::option::GimmickValueCheckSettings::Param {
    enum class CompareMode : int64_t {
        Equal = 0,
        NotEqual = 1,
        Bigger = 2,
        Smaller = 3,
        EqualBigger = 4,
        EqualSmaller = 5,
    };
}
namespace app::ropeway::EnvironmentStandbyManager {
    enum class AreaUpdateResult : int64_t {
        Processed = 0,
        SkipByNoPlayer = 1,
        SkipByGimmickPause = 2,
        SkipBySameArea = 3,
        SkipByNoController = 4,
        SkipByPlayerJack = 5,
        SkipByInvalidMap = 6,
        SkipByLoadStopRequest = 7,
        SkipByNoDirectJumpRequest = 8,
    };
}
namespace app::ropeway::doorpry {
    enum class OpenAngleType : int64_t {
        OPEN_90 = 0,
        OPEN_150 = 1,
    };
}
namespace via {
    enum class ShareServiceTicketStatus : int64_t {
        Invalid = 0,
        Requested = 1,
        Running = 2,
        Finished = 3,
    };
}
namespace via::effect {
    enum class PlayerType : int64_t {
        Effect = 0,
        Lensflare = 1,
    };
}
namespace app::ropeway::gamemastering::Item {
    enum class Category : int64_t {
        Invalid = 0xFFFFFFFF,
        Weapon = 0,
        Custom = 1,
        Material = 2,
        Bullet = 3,
        Heal = 4,
        Support = 5,
        Key = 6,
        Etc = 7,
        MAX = 8,
    };
}
namespace app::ropeway::gamemastering::RecordManager {
    enum class RewardType : int64_t {
        INVALID = 0,
        FIGURE = 1,
        CONCEPTART = 2,
        MODE = 3,
        WEAPON = 4,
        COSTUME = 5,
    };
}
namespace app::ropeway::enemy::em5000::RoleMotionID {
    enum class Hang : int64_t {
        HANG_00 = 4200,
    };
}
namespace via::gui {
    enum class FadeMode : int64_t {
        IN_ = 0,
        Wait = 1,
        Out = 2,
        Invisible = 3,
        End = 4,
    };
}
namespace via::render::DepthOfField {
    enum class DepthOfFieldBokehControl : int64_t {
        Default = 0,
        IgnoreNear = 1,
        IgnoreFar = 2,
        DepthOfFieldBokehControle_Max = 3,
    };
}
namespace app::ropeway::timeline::PerformanceInfo {
    enum class DispEquipState : int64_t {
        WAIT_HIDE = 0,
        WAIT_DISP = 1,
        END = 2,
    };
}
namespace via::effect::detail {
    enum class PtColorOperator : int64_t {
        Overwrite = 0,
        Multiply = 1,
    };
}
namespace app::ropeway::enemy::common::fsmv2::condition::LastDamageDirection {
    enum class CrossDir : int64_t {
        FrontOrBack = 0,
        LeftOrRight = 1,
    };
}
namespace app::ropeway::enemy::em9000::Em9000Think {
    enum class HandLookAtType : int64_t {
        LEFT_HAND_HANDLIGHT = 0,
        RIGHT_HAND_FOUND = 1,
    };
}
namespace app::ropeway::enemy::em6200 {
    enum class RequestSetID : int64_t {
        PRESS = 0,
        DAMAGE = 1,
        SENSOR_TOUCH = 2,
        SENSOR_GIMMICK = 3,
        SENSOR_ENEMY = 4,
        ATTACK_HOOK_R = 5,
        ATTACK_HOOK_L = 6,
        ATTACK_PUNCH_R = 7,
        ATTACK_PUNCH_L = 8,
        ATTACK_HAMMER = 9,
        ATTACK_TURN_BLOW_L = 10,
        ATTACK_TURN_BLOW_R = 11,
        ATTACK_FLASH = 12,
        TANGLE_PUSH_R = 13,
        TANGLE_PUSH_L = 14,
        TANGLE_KICK_R = 15,
        TANGLE_KICK_L = 16,
        TANGLE_PUNCH = 17,
        GLAPPLE_HOLD = 18,
        GLAPPLE_THROW = 19,
        GLAPPLE_DOWNCRUSH = 20,
        CAMERA_DRAW_CONTROL = 21,
        ATTACK_FALL = 22,
        TANGLE_FALL = 23,
        AIM_TARGET = 24,
    };
}
namespace app::ropeway::enemy::em6000::fsmv2::action::Em6000FsmAction_SetPusEmissive {
    enum class EXEC_TYPE : int64_t {
        Start = 0,
        End = 1,
        Sequence = 2,
    };
}
namespace app::ropeway::IkHumanRetargetSolver {
    enum class RetargetTargetType : int64_t {
        Self = 0,
        Target1 = 1,
        Target2 = 2,
    };
}
namespace app::ropeway::enemy::em7000::Em7000Define {
    enum class BtActionStatus : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 2,
        ACT_WAIT_ATTACK = 3,
        ACT_TO_G = 4,
        ACT_TO_W = 5,
        ACT_TO_START_LEON = 6,
        ACT_WAIT_BATTLE = 7,
        ACT_WALK_FSM = 9,
        ACT_BATTLE_START = 10,
        ATK_W_SHORT_R = 2000,
        ATK_W_CONTINUOUS_2 = 2001,
        ATK_W_CONTINUOUS_3 = 2002,
        ATK_W_MIDDLE = 2003,
        ATK_G_SHORT_R = 2004,
        ATK_G_SHORT_F = 2005,
        ATK_G_CONTINUOUS_MIDDLE = 2006,
        ATK_G_GRAPPLE = 2007,
        ATK_W_LARGE_A = 2008,
        ATK_W_CONTINUOUS_A = 2009,
        ATK_W_CONTINUOUS_B = 2010,
        ATK_W_COUTINUOUS_WALK = 2011,
        ATK_G_SHORT_R_FORWARD = 2012,
        ATK_G_SHORT_F_FORWARD = 2013,
        ATK_G_LARGE_A = 2014,
        ATK_G_WALK_ATTACk = 2015,
        ATK_G_PUSH_ATTACK = 2016,
        ATK_W_GRAPPLE = 2017,
    };
}
namespace app::ropeway::GameRankSystem {
    enum class OnceAddBitFlag : int64_t {
        BOSS_G1 = 0,
        BOSS_G2 = 1,
        BOSS_G3 = 2,
        BOSS_G35 = 3,
        BOSS_G5 = 4,
        BOSS_TYRANT = 5,
        BOSS_N_TYRANT = 6,
    };
}
namespace app::ropeway::enemy::em6300 {
    enum class RequestSetID : int64_t {
        INVALID = 0xFFFFFFFF,
        PRESS = 0,
        DAMAGE = 1,
        SENSOR_TOUCH = 2,
        SENSOR_GIMMICK = 3,
        ATTACK_RIGHT_NAIL = 4,
        ATTACK_LEFT_NAIL = 5,
        ATTACK_SMALL_RIGHT_NAIL = 6,
        ATTACK_SMALL_LEFT_NAIL = 7,
        ATTACK_THRUST = 8,
        ATTACK_NAIL_UPPER = 9,
        ATTACK_GARIGARI_NAIL_UPPER = 10,
        ATTACK_POWER_DUNK = 11,
        ATTACK_POWER_DUNK_AFTER = 12,
        AIM_TARGET = 13,
    };
}
namespace app::ropeway::WwiseGlobalUserVariablesTriggerListForLevel::TriggerElementForLevel {
    enum class PositionType : int64_t {
        Position = 0,
        Follow = 1,
    };
}
namespace app::ropeway::enemy {
    enum class MonitorSupportItemActionType : int64_t {
        Invoke = 0,
        Consume = 1,
        Action = 2,
        NUM = 3,
    };
}
namespace app::ropeway::enemy::em4400::BehaviorTreeAction {
    enum class ID : int64_t {
        ACT_IDLE = 0,
    };
}
namespace app::ropeway::gimmick::action::InteractManager {
    enum class FilterlingLevel : int64_t {
        Event_F = 0,
        Event_E = 99,
        GetItem_F = 100,
        GetItem_E = 199,
        GimmickEx_F = 200,
        GimmickEx_E = 299,
        Gimmick_F = 300,
        Gimmick_E = 399,
        Max_F = 9999,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::HideDeadEnemys {
    enum class MotSeqCheckTarget : int64_t {
        FsmOwner = 0,
        MotionJackTarget = 1,
    };
}
namespace app::ropeway::enemy::em3000 {
    enum class NavigationFilterGroup : int64_t {
        GROUND = 0,
        WALL = 1,
        CEILING = 2,
        WALL_FORCE_WALKABLE = 3,
        CEILING_FORCE_WALKABLE = 4,
        WALL_THROUGH = 5,
        WALL_THROUGH_CAUTION = 6,
    };
}
namespace app::ropeway::gui::FloorMapSt41Behavior {
    enum class BgObjectId : int64_t {
        BgObject_invalid = 0,
        sm40_033_ZombieWindow01A_1FW1 = 1,
        sm49_023_KeepoutWindow = 2,
        sm40_048_ZombieWindow01B_1FWCorridorAoku = 3,
        sm40_033_ZombieWindow01A_1FW4 = 4,
        sm40_033_ZombieWindow01A_OfficeE3 = 5,
        sm40_033_ZombieWindow01A_OfficeE4 = 6,
        sm40_033_ZombieWindow01A_1FE1 = 7,
        sm40_033_ZombieWindow01A_1FE2 = 8,
        sm40_048_ZombieWindow01B_1FECorridorC1 = 9,
        sm40_048_ZombieWindow01B_1FECorridorC2 = 10,
        sm40_048_ZombieWindow01B_1FECorridorC3 = 11,
        sm40_048_ZombieWindow01B_1FECorridorC4 = 12,
        Barricade_RPD1_00 = 13,
        Barricade_RPD1_01 = 14,
        Barricade_RPD1_02 = 15,
        Barricade_RPD1_03 = 16,
    };
}
namespace via::render::ToneMapping {
    enum class Vignetting : int64_t {
        Enable = 0,
        KerarePlus = 1,
        Disable = 2,
    };
}
namespace via::navigation {
    enum class DebugDrawAttribute : int64_t {
        None = 0,
        Fill = 1,
    };
}
namespace mpl_::assert_ {
    enum class relations : int64_t {
        equal = 1,
        not_equal = 2,
        greater = 3,
        greater_equal = 4,
        less = 5,
        less_equal = 6,
    };
}
namespace app::ropeway::GimmickRaccoonFigureManager {
    enum class RaccoonID : int64_t {
        Invalid = 0xFFFFFFFF,
        Raccoon_0 = 0,
        Raccoon_1 = 1,
        Raccoon_2 = 2,
        Raccoon_3 = 3,
        Raccoon_4 = 4,
        Raccoon_5 = 5,
        Raccoon_6 = 6,
        Raccoon_7 = 7,
        Raccoon_8 = 8,
        Raccoon_9 = 9,
        Raccoon_10 = 10,
        Raccoon_11 = 11,
        Raccoon_12 = 12,
        Raccoon_13 = 13,
        Raccoon_14 = 14,
        Raccoon_15 = 15,
        Raccoon_16 = 16,
        Raccoon_17 = 17,
        Raccoon_18 = 18,
        Raccoon_19 = 19,
        NUM = 20,
    };
}
namespace app::ropeway::enemy::em7100 {
    enum class CounterIndex : int64_t {
        CounterIndex_00 = 0,
        CounterIndex_01 = 1,
        CounterIndex_02 = 2,
        CounterIndex_03 = 3,
        CounterIndex_Max = 4,
    };
}
namespace via::os::PackManager {
    enum class HandleUsage : int64_t {
        Read = 0,
        StartPrefetch = 1,
        QueryCache = 2,
    };
}
namespace app::ropeway::CollisionDefine {
    enum class AttackSortType : int64_t {
        Distance = 0,
        Priority = 1,
    };
}
namespace app::ropeway::enemy::em5000 {
    enum class WeakPartsGroup : int64_t {
        UpLeft = 0,
        UpRight = 1,
        DownLeft = 2,
        DownRight = 3,
    };
}
namespace via::clr {
    enum class EnumU1 : int64_t {
        Dummy = 0,
    };
}
namespace app::ropeway::gui::NewInventoryBehavior {
    enum class CloseTrigger : int64_t {
        None = 0,
        InventoryButton = 1,
        MapButton = 2,
    };
}
namespace app::ropeway::FigureManager {
    enum class FigureLightType : int64_t {
        Normal = 0,
        Small = 1,
        em_Normal = 2,
        em_Big = 3,
        em6400 = 4,
        em7400 = 5,
        Invalid = 6,
    };
}
namespace app::ropeway::gamemastering::SaveDataManager {
    enum class LastErrorType : int64_t {
        NONE = 0,
        SYSTEM_SAVE = 1,
        AUTO_SAVE = 2,
        SAVE = 3,
        LOAD = 4,
    };
}
namespace via::os::dialog {
    enum class Result : int64_t {
        None = 0,
        Busy = 1,
        Ok = 2,
        RightButton = 3,
        LeftButton = 4,
        Cancel = 5,
        Abort = 6,
        ERROR_ = 7,
    };
}
namespace app::ropeway::gui::GimmickKeyInputGuiBehavior {
    enum class KeyType : int64_t {
        NONE = 0xFFFFFFFF,
        ABC = 0,
        DEF = 1,
        GHI = 2,
        JKL = 3,
        MNO = 4,
        PQR = 5,
        STU = 6,
        VWX = 7,
        YZ = 8,
        BACK = 9,
        ZERO = 10,
        ENTER = 11,
        MAX = 12,
    };
}
namespace app::ropeway::gamemastering::WaitStaffRollFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        UPDATE = 1,
        FINALIZE = 2,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace via::motion::IkFourLeg {
    enum class RayCastSkipOption : int64_t {
        None = 0,
        DIV2 = 2,
        DIV4 = 4,
    };
}
namespace app::ropeway::enemy::savedata::EmSaveData {
    enum class TransformDataIndex : int64_t {
        POSITION_X = 0,
        POSITION_Y = 1,
        POSITION_Z = 2,
        ROTATION_X = 3,
        ROTATION_Y = 4,
        ROTATION_Z = 5,
        ROTATION_W = 6,
        FIRST_POSITION_X = 7,
        FIRST_POSITION_Y = 8,
        FIRST_POSITION_Z = 9,
        __ARRAY_SIZE__ = 10,
    };
}
namespace app::ropeway::InputDefine {
    enum class Category : int64_t {
        ACTION = 0,
        GIMMICK = 1,
        SYSTEM = 2,
        SHORTCUT = 3,
    };
}
namespace app::ropeway::weapon::shell::BallisticSettingBase {
    enum class BallisticEndType : int64_t {
        INVALID = 0,
        OVER_CARRY = 1,
        HIT_CHARACTER = 2,
        HIT_TERRAIN_ = 3,
        OVER_TIME = 4,
    };
}
namespace app::ropeway::motfsm::fsmv2::MotFSM {
    enum class CheckTimingType : int64_t {
        ALWAYS = 0,
        MOTION_END = 1,
        FRAME = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class BreakPartsReason : int64_t {
        INVALID = 0,
        IMMIDIATE = 1,
        AGAIN_ = 2,
        DELAY = 3,
        CANCELED = 4,
    };
}
namespace via::DateTime {
    enum class Comparison : int64_t {
        Full = 0,
        FullAbout = 1,
        Date = 2,
        Time = 3,
        TimeAbout = 4,
    };
}
namespace via::attribute {
    enum class KeyComparisonType : int64_t {
        None = 0,
        Equal = 1,
        NotEqual = 2,
        Less = 3,
        LessEqual = 4,
        Greater = 5,
        GreaterEqual = 6,
    };
}
namespace via::userdata::CalculateNode {
    enum class Operation : int64_t {
        Add = 0,
        Sub = 1,
        Mul = 2,
        Div = 3,
        Mod = 4,
        Pow = 5,
        MIN_ = 6,
        Max = 7,
    };
}
namespace app::ropeway::enemy::EnemyWeaponDataUserData::WeaponValueInfoBase {
    enum class SpecialShellType : int64_t {
        Burn = 0,
        BurnUp = 1,
        Acid = 2,
        Roast = 3,
        Tangle = 4,
        Unknown = 5,
    };
}
namespace app::ropeway::timeline::TimelineEventSettingTable::CameraSetting {
    enum class ToPositionType : int64_t {
        JOINT = 0,
        PREV_EVENT = 1,
    };
}
namespace via::motion::IkJacobian::Effector {
    enum class AttributeType : int64_t {
        Position = 1,
        Rotation = 2,
        Animation = 4,
        TargetObj = 8,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class TurnOverStatus : int64_t {
        FACEUP = 0,
        TURNOVER = 1,
        FACEDOWN = 2,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class CONDITION_TYPE : int64_t {
        Burn = 0,
        BurnUp = 1,
        Acid = 2,
        Electric = 3,
        Blind = 4,
        BiteGrenade = 5,
        StickProbe = 6,
        Roast = 7,
    };
}
namespace System::Reflection {
    enum class PropertyAttributes : int64_t {
        None = 0,
        SpecialName = 512,
        RTSpecialName = 1024,
        HasDefault = 4096,
        Reserved2 = 8192,
        Reserved3 = 16384,
        Reserved4 = 32768,
        ReservedMask = 62464,
    };
}
namespace via::network::session {
    enum class SearchByFriendOption : int64_t {
        None = 0,
        CreatorOnly = 2,
    };
}
namespace app::ropeway::timeline::TimelinePreparation::PresetInfo {
    enum class Type : int64_t {
        InGame = 0,
        EventOnly = 1,
    };
}
namespace app::ropeway::gui::SelectLanguageBehavior {
    enum class State : int64_t {
        VOICE_LANGUAGE = 0,
        TEXT_LANGUAGE = 1,
        CAPTION = 2,
        SPEAKER_TYPE = 3,
        BINAURAL = 4,
    };
}
namespace app::ropeway::gimmick::option::GimmickCameraSettings::CameraSetting {
    enum class ConstType : int64_t {
        TARGET_OBJECT = 0,
        PLAYER = 1,
        NONE = 2,
    };
}
namespace via::areamap::test::DistanceFromItemWithTag {
    enum class Condition : int64_t {
        Mode_Closest = 0,
        Mode_Farthest = 1,
    };
}
namespace app::ropeway {
    enum class RegionType : int64_t {
        Unknown = 0,
        Japan = 1,
        USA = 2,
        Europe = 3,
        Asia = 4,
    };
}
namespace app::ropeway::effect::script::DestructEffectHelper {
    enum class TargetParts : int64_t {
        Head = 0,
        Arm = 1,
        Leg = 2,
        Body = 3,
        Head_DisableMeat = 4,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0100_MONTAGE_PARTS_FACE : int64_t {
        FACE00 = 0,
        FACE01 = 1,
        FACE02 = 2,
        FACE03 = 3,
        FACE04 = 4,
        NONE = 5,
    };
}
namespace app::ropeway::gimmick::action::GimmickWordBlock {
    enum class ActionResult : int64_t {
        NOT_YET = 0,
        END = 1,
        FAILED = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickFussyAttack {
    enum class State : int64_t {
        STANDBY = 0,
        ATTACK = 1,
        STOP = 2,
    };
}
namespace via::fsm::Selector {
    enum class Attribute : int64_t {
        AttributeSelectBeforeAction = 0,
        AttributeBitNum = 32,
    };
}
namespace app::ropeway::enemy::em7000::Em7000Define {
    enum class MotionFsmLayer : int64_t {
        MotionFsmLayer_Action = 0,
        MotionFsmLayer_DamageAdd = 1,
        MotionFsmLayer_ArmREye = 2,
        MotionFsmLayer_ArmScaleAdd = 3,
    };
}
namespace app::ropeway::enemy::em6300::Em6300Think {
    enum class STATE : int64_t {
        LookAt = 0,
        Walk = 1,
        Dash = 2,
        DashStagger = 3,
        AttackDisable = 4,
    };
}
namespace app::ropeway::rogue::RogueEnemyJointController {
    enum class PlayTiming : int64_t {
        Invalid = 0,
        OnGrappleStart = 1,
        OnGrappleEnd = 2,
    };
}
namespace app::ropeway::enemy::DropPartsRequestBase {
    enum class RequestStatus : int64_t {
        Invalid = 0,
        WaitInstantiate = 1,
        WaitSetup = 2,
        WaitActivate = 3,
        Completed = 4,
        Failed = 5,
    };
}
namespace app::ropeway::CollisionDefine {
    enum class AvoidMarkerType : int64_t {
        NORMAL = 0,
    };
}
namespace via::wwise {
    enum class EventTargetType : int64_t {
        GameObject = 0,
        EntireComponent = 1,
    };
}
namespace via::path {
    enum class DriveType : int64_t {
        App = 0,
        SaveData = 1,
        AddContent = 2,
        Download = 3,
        Temp = 4,
        Host = 5,
        HostApp = 6,
        DevKit = 7,
        Home = 8,
        MaxType = 9,
    };
}
namespace via {
    enum class VideoRecordingStopOption : int64_t {
        SaveFile = 0,
        SaveFileAndExportLibrary = 1,
        Discard = 2,
    };
}
namespace app::ropeway::SurvivorDefine::Restriction {
    enum class Layer : int64_t {
        Invalid = 0,
        PlayerAction = 1,
        Enemy = 2,
        PlayerJackBase = 3,
        Event = 4,
        RestrictManager = 5,
        InteractManager = 6,
        GimmickEx = 7,
        Npc = 8,
        Debug = 9,
    };
}
namespace System::Reflection {
    enum class MemberTypes : int64_t {
        Constructor = 1,
        Event = 2,
        Field = 4,
        Method = 8,
        Property = 16,
        TypeInfo = 32,
        Custom = 64,
        NestedType = 128,
        All = 191,
    };
}
namespace via::motion::JointData {
    enum class BlendMode : int64_t {
        Default = 0,
        Root = 1,
    };
}
namespace app::ropeway::gimmick::action::InteractManager {
    enum class ExecutablePriority : int64_t {
        Event = 99,
        GetItem = 199,
        GimmickEx = 299,
        Gimmick = 399,
    };
}
namespace via::effect::script::EffectManager {
    enum class PriorigyGroup : int64_t {
        PriorityGroup_Default = 0,
        PriorityGroup_RadialBlur = 1,
        PriorityGroup_DirtMask = 2,
        PriorityGroup_ColorCorrect = 3,
        PriorityGroup_Num = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickSkewLift {
    enum class RequestType : int64_t {
        DOWN_START = 0,
        DOWN_END = 1,
        RISE_START = 2,
        RISE_END = 3,
        MAX = 4,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0200_MONTAGE_PARTS_SHIRT : int64_t {
        NONE = 0,
        SHIRT00 = 1,
        SHIRT00_00_00 = 2,
        SHIRT00_01_00 = 3,
        SHIRT00_02_00 = 4,
        SHIRT00_03_00 = 5,
        SHIRT00_04_00 = 6,
        SHIRT01 = 7,
        SHIRT02 = 8,
        SHIRT03 = 9,
        SHIRT04 = 10,
        SHIRT70 = 11,
    };
}
namespace app::ropeway::HandHeldItem {
    enum class EquipStatus : int64_t {
        INVALID = 0,
        EQUIPED = 1,
        HOLSTER = 2,
        INVISIBLE = 3,
    };
}
namespace app::ropeway::gui::FloorMapSt53Behavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_5_223_0 = 1,
    };
}
namespace app::ropeway::Em4000Define {
    enum class RankParamHash : int64_t {
        NO_ATTACK_TIME = 3730143395,
        NO_HOLD_TIME = 470028201,
        NO_BACK_HOLD_TIME = 3236473812,
        REACTION_RATE = 2430824357,
        REACTION_STOP_TIME = 2950829043,
    };
}
namespace via::motion::detail::IkFingerSolver {
    enum class fingerType : int64_t {
        Thumb = 0,
        Normal = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickGoddessStatue {
    enum class MedalType : int64_t {
        VIRGIN_ = 0,
        UNICORN = 1,
        LION = 2,
        MAX = 3,
    };
}
namespace app::ropeway {
    enum class SimpleAreaShapeType : int64_t {
        Invalid = 0,
        Sphere = 1,
        Cylinder = 2,
        Box = 3,
    };
}
namespace app::ropeway::enemy::em0000::fsmv2::action::Em0000MFsmAction_SetMotionPattern {
    enum class SetMethodType : int64_t {
        DIRECT = 0,
        RANDOM = 1,
        ROLE_MOTION_ID = 2,
        ROLE_MOTION_ID_RAW = 3,
        TARGET_FB = 4,
        TARGET_LR = 5,
        TARGET_SIDE_FB = 6,
        TARGET_SIDE_LR = 7,
        TARGET_HOLDFROM_LR = 8,
        TARGET_HOLDFROM_LR_BY_SECOND = 9,
        UNDERWATER = 10,
        LAST_DAMAGE_FB = 11,
        LAST_DAMAGE_LR = 12,
        INPUT_3WAYDIR_FROM_PLAYER = 13,
    };
}
namespace app::ropeway::gimmick::action::GimmickCovertPlace {
    enum class ActionType : int64_t {
        NONE = 0,
        HIDE = 1,
        HIDING = 2,
        UNCOVER = 3,
    };
}
namespace app::ropeway::survivor::fsmv2::condition::ActionOrderCondition {
    enum class StateEndType : int64_t {
        Order = 0,
        OrderOrMotionEnd = 1,
        OrderAndMotionEnd = 2,
        OrderAndActionTimer = 3,
        OrderAndVariable = 4,
        OrderOrUnderLayerEnd = 5,
        OrderAndUnderLayerEnd = 6,
        None = 0,
        MotionEnd = 2,
        ActionTimer = 3,
        Variable = 4,
        NONE = 0,
        EXIT_MOTION_END = 2,
        EXIT_ACT_TIMER = 3,
        EXIT_VARIABLE = 4,
    };
}
namespace via::fsm {
    enum class ExecGroup : int64_t {
        ExecGroup_00 = 0,
        ExecGroup_01 = 1,
        ExecGroup_02 = 2,
        ExecGroup_03 = 3,
        ExecGroup_04 = 4,
        ExecGroup_05 = 5,
        ExecGroup_06 = 6,
        ExecGroup_07 = 7,
    };
}
namespace app::ropeway::gimmick::action::GimmickCustomJackControl {
    enum class InteractKeyType : int64_t {
        KEY_DOWN = 0,
        KEY_KEEP_ON = 1,
    };
}
namespace via::timeline {
    enum class PlayState : int64_t {
        PlayStart = 0,
        Play = 1,
        Stop = 2,
        Pause = 3,
        End = 4,
        HalfwayPlay = 5,
        PlayEnd = 6,
        Restore = 7,
    };
}
namespace app::ropeway::gui::OptionResultBehavior {
    enum class MemoryState : int64_t {
        NORMAL = 0,
        HALF_OVER = 1,
        LIMIT_OVER = 2,
    };
}
namespace via::hid::hmd::MorpheusDevice {
    enum class Status : int64_t {
        Sleep = 0,
        Setup = 1,
        Standby = 2,
        Startup = 3,
        Active = 4,
        Cleanup = 5,
    };
}
namespace via::physics {
    enum class FillMode : int64_t {
        Solid = 0,
        WireFrame = 1,
    };
}
namespace via::render {
    enum class BlendOp : int64_t {
        Add = 1,
        Subtract = 2,
        RevSubtract = 3,
        MIN_ = 4,
        Max = 5,
        Num = 6,
    };
}
namespace via::motion {
    enum class StartType : int64_t {
        Frame = 0,
        NormalizedTime = 1,
        SyncTime = 2,
        AutoSyncTime = 3,
        AutoSyncTimeSamePointCount = 4,
    };
}
namespace via::motion::IkDamageAction {
    enum class DebugDamageTransition : int64_t {
        Non = 0,
        ImpactState = 1,
        MaxState = 2,
    };
}
namespace via::render::DepthOfField {
    enum class DepthOfFieldType : int64_t {
        Default = 0,
        Tessellation = 1,
        Lite = 2,
        Max = 3,
    };
}
namespace app::ropeway::ActionTargetController {
    enum class TargetModeType : int64_t {
        None = 0,
        GameObject = 1,
        Position = 2,
    };
}
namespace app::ropeway::gui::TitleABehavior {
    enum class DecidedResult : int64_t {
        DUMMY = 0,
        MAX = 1,
        CANCEL = 2,
    };
}
namespace app::ropeway::SceneStandbyController {
    enum class Status : int64_t {
        None = 0,
        Standby = 1,
        Active = 2,
    };
}
namespace app::ropeway::survivor::npc::NpcOrderDefine {
    enum class InsteadActionKind : int64_t {
        Walk2Jog = 0,
        Jog2Walk = 1,
        Invalid = 2,
    };
}
namespace via::motion::JointExMultiRemapValue::OutputData {
    enum class TransformElement : int64_t {
        Trans = 0,
        Rot = 1,
        Scale = 2,
        Deform = 3,
        RotRPY = 4,
        RotPYR = 5,
        ExpMap = 6,
    };
}
namespace via::motion {
    enum class MotionFrameControl : int64_t {
        Normal = 0,
        SyncBaseLayerNormalizeTime = 1,
        PauseStartFrame = 2,
        PauseEndFrame = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickAutoDoor {
    enum class DoorSideType : int64_t {
        SIDE_A = 0,
        SIDE_B = 1,
        MAX = 2,
    };
}
namespace app::ropeway::interpolate::InterpolateTiltRequest {
    enum class TiltMode : int64_t {
        MANUAL = 0,
        AXIS_TO_TILT = 1,
        TILT_TO_AXIS = 2,
    };
}
namespace via::motion {
    enum class TreeLayerMode : int64_t {
        None = 0,
        Motion = 1,
        Camera = 2,
    };
}
namespace app::ropeway::enemy::em6300 {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        AttackRightNail = 4,
        AttackLeftNail = 5,
        AttackSmallRightNail = 6,
        AttackSmallLeftNail = 7,
        AttackThrust = 8,
        AttackNailUpper = 9,
        AttackGariGariNailUpper = 10,
        AttackPowerDunk = 11,
        AttackPowerDunkAfter = 12,
    };
}
namespace app::ropeway::RagdollControlZone {
    enum class ZoneKind : int64_t {
        EntryProhibition = 0,
        Freeze = 1,
    };
}
namespace app::ropeway::enemy::em7300::tracks::Em7300EffectTrack {
    enum class EffType : int64_t {
        STAGE_EFF_0 = 0,
        G4_LANDING_EFF = 1,
    };
}
namespace via::gui {
    enum class ReprojectionType : int64_t {
        Default = 0,
        WithOverlay = 1,
    };
}
namespace via::render::layer::Transparent {
    enum class ReducedTransparentBuffer : int64_t {
        Eighth = 0,
        Quat = 1,
        Default = 2,
        Half = 3,
        Full = 4,
        Max = 5,
    };
}
namespace app::ropeway::OptionManager {
    enum class TextureQuality : int64_t {
        LOW = 0,
        STANDARD_025 = 1,
        STANDARD_05 = 2,
        HIGH_025 = 3,
        HIGH_05 = 4,
        HIGH_1 = 5,
        HIGH_2 = 6,
        HIGH_3 = 7,
        HIGH_4 = 8,
        HIGH_6 = 9,
        HIGH_8 = 10,
    };
}
namespace app::ropeway::enemy::em6200::Em6200Think {
    enum class STATE : int64_t {
        PushAction = 0,
        DoorAccessEnable = 1,
        DoorOpening = 2,
        SightChecked = 3,
        ScreenTaregt = 4,
        ToWalkContinue = 5,
        NoCansel = 6,
        CrouchEnable = 7,
        Encount = 8,
        NotifyGimmickDoor = 9,
        GimmickDoorChecked = 10,
        GimmickDoorRequest = 11,
        PLPosHigh = 12,
        PLUseLadder = 13,
        LaddarWait = 14,
        AttackSightCheck = 15,
        ActionInterEnable = 16,
        IgnoreChaserCancelOnWarp = 17,
        CheckWalkFoot = 18,
    };
}
namespace via::browser::utility::Request {
    enum class State : int64_t {
        Dead = 0,
        Start = 1,
        StartFail = 2,
        Update = 3,
        Finish = 4,
    };
}
namespace via::physics::DynamicBoundingVolumeHierarchy {
    enum class ErrorCode : int64_t {
        None = 0,
        FunctorAbort = 1,
        Max = 2,
    };
}
namespace via::physics {
    enum class ShapeClosestOption : int64_t {
        AllHits = 0,
        DisableBackFacingTriangleHits = 1,
        DisableFrontFacingTriangleHits = 2,
        NearSort = 3,
        OneHitBreak = 4,
        Max = 5,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class PlayerManagePhase : int64_t {
        FASTEST = -51,
        Player = -50,
        Camera = -49,
        LATEST = -48,
    };
}
namespace app::ropeway::SoundListenerController {
    enum class Type : int64_t {
        LEON = 0,
        CAMERA = 1,
        LEON_CAMERA = 2,
    };
}
namespace via::effect::script::EffectEmitZoneGroup {
    enum class State : int64_t {
        None = 0,
        EmitOffTrigger = 1,
        EmitOnTrigger = 2,
        EmitOn = 3,
    };
}
namespace via::render {
    enum class BindFlag : int64_t {
        ShaderResource = 8,
        RenderTarget = 32,
        DepthStencil = 64,
        UnorderedAccess = 128,
    };
}
namespace via::clr {
    enum class MethodCodeFlag : int64_t {
        FatFormat = 3,
        TinyFormat = 2,
        MoreSects = 8,
        InitLocals = 16,
    };
}
namespace app::ropeway::enemy::em5000 {
    enum class HoldTargetStatus : int64_t {
        IDLE = 0,
        MOVE = 1,
        TOWARD = 2,
        GRAPPLE = 3,
    };
}
namespace via::hid::VrTracker {
    enum class TrackerDevicePermitType : int64_t {
        All = 0,
        HmdOnly = 1,
    };
}
namespace app::ropeway::Em6000Think {
    enum class CONV_STATE : int64_t {
        None = 0,
        Ready = 1,
        Timer = 2,
        Grapple = 3,
        Finish = 4,
    };
}
namespace app::ropeway::gui::RogueInventorySlotBehavior {
    enum class SlotSelectMode : int64_t {
        Normal = 0,
        Combination = 1,
        ExchangeCombination = 2,
        GetItem = 3,
        UseItem = 4,
        ItemBox = 5,
        Shortcut = 6,
        Detail = 7,
        ExCombination = 8,
    };
}
namespace app::ropeway::NpcDefine {
    enum class MoveEndAngle : int64_t {
        NONE = 0,
        ANY = 1,
        PLAYER = 2,
    };
}
namespace app::ropeway::ParamCurveAnimator::ProcessPos {
    enum class TargetType : int64_t {
        POS_X = 0,
        POS_Y = 1,
        POS_Z = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0000_MONTAGE_PARTS_HAT : int64_t {
        HAT00 = 0,
        HAT01 = 1,
        NONE = 2,
    };
}
namespace app::ropeway::camera::InterpolationCameraEndResult {
    enum class ResultType : int64_t {
        DONE = 0,
        CANCELED = 1,
    };
}
namespace app::ropeway::enemy::EnemyRelocationPointInfo {
    enum class PRIORITY : int64_t {
        Highest = 0,
        High = 1,
        Normal = 2,
        Low = 3,
        Lowest = 4,
    };
}
namespace via::motion::IkHand {
    enum class CalculationType : int64_t {
        CalculationType_Easy = 0,
        CalculationType_2Bone = 1,
        CalculationType_FitGround = 2,
    };
}
namespace app::ropeway::EffectReceiverFromGimmick {
    enum class EffectType : int64_t {
        OBJECT = 0,
        ENVIRONMENT = 1,
    };
}
namespace app::ropeway::fsmv2::action::variables::SetVariablesAction {
    enum class SetTimingType : int64_t {
        OnStart = 0,
        OnUpdate = 1,
        OnExit = 2,
    };
}
namespace app::ropeway::environment::LocationController {
    enum class Phase : int64_t {
        RegistLocation = 0,
        None = 1,
        NoneToStandby = 2,
        StandbyToNone = 3,
        Standby = 4,
        StandbyToActive = 5,
        StandbyToActiveScenario = 6,
        ActiveToStandby = 7,
        ActiveToStandbyScenario = 8,
        Active = 9,
    };
}
namespace via::effect::detail {
    enum class LifeState : int64_t {
        Unknown = 0,
        Wait = 1,
        Initialize = 2,
        Appear = 3,
        Keep = 4,
        KeepHold = 5,
        Vanish = 6,
        Terminate = 7,
    };
}
namespace app::ropeway::enemy::em6000::fsmv2::action::Em6000FsmAction_SetMotionPattern {
    enum class PATTERN_TYPE : int64_t {
        TargetLR = 0,
        InWater = 1,
        Role = 2,
        WalkStart = 3,
        Stun = 4,
    };
}
namespace app::ropeway::gamemastering::SaveDataManager {
    enum class SaveLoadStep : int64_t {
        INITIALIZE = 0,
        REQUEST_WAIT = 1,
        SAVE_WAIT = 2,
        LOAD_WAIT = 3,
        REMOVE_DATA_WAIT = 4,
        ERROR_START_INDEX = 5,
        ERROR_WAIT_DIALOG = 6,
        ERROR_SYSTEM_START = 7,
        ERROR_SYSTEM_NOTICE = 8,
        ERROR_SYSTEM_SELECT = 9,
        ERROR_SYSTEM_AGAIN_ = 10,
        ERROR_SYSTEM_END = 11,
        ERROR_AUTO_START = 12,
        ERROR_AUTO_NOTICE = 13,
        ERROR_AUTO_SELECT = 14,
        ERROR_AUTO_AGAIN_ = 15,
        ERROR_AUTO_SUCCESS_END = 16,
        ERROR_AUTO_FAILURE_END = 17,
        ERROR_GAME_START = 18,
        ERROR_GAME_END = 19,
        ERROR_LOAD_START = 20,
        ERROR_LOAD_END = 21,
    };
}
namespace via::wwise {
    enum class RequestType : int64_t {
        None = 0,
        RegisterGameObject = 1,
        UnregisterGameObject = 2,
        SetActiveListeners = 3,
        SetListenerPosition = 4,
        Set3dPosition = 5,
        SetRtpcValue = 6,
        SetState = 7,
        SetSwitch = 8,
        PostEvent = 9,
        SeekOnEvent = 10,
        PostTrigger = 11,
        StopPlayingId = 12,
        StopAll = 13,
        SetGameObjectOutputBusVolume = 14,
        SetGameObjectAuxSendValues = 15,
        SetAttenuationScalingFactor = 16,
        SetObjectObstructionAndOcclusion = 17,
        AddSecondaryOutput = 18,
        RemoveSecondaryOutput = 19,
        SetSpeakerAngles = 20,
        RegisterEmitter = 21,
        UnregisterEmitter = 22,
        AddPortal = 23,
        RemovePortal = 24,
        AddRoom = 25,
        RemoveRoom = 26,
        SetGameObjectInRoom = 27,
        Max = 28,
    };
}
namespace app::ropeway::gui::GimmickNumberLockHothouseGuiBehavior2 {
    enum class UnderLineType : int64_t {
        DEFAULT = 0,
        INPUT = 1,
        DISABLE = 2,
    };
}
namespace app::ropeway::render::StreamingTextureManager {
    enum class RequestType : int64_t {
        None = 0,
        StandbyMediator = 1,
        TimelineManager = 2,
    };
}
namespace via::storage::saveService {
    enum class SaveServiceSegmentType : int64_t {
        Default_0 = 0,
        Default_1 = 1,
        Default_2 = 2,
        Default_3 = 3,
        Default_4 = 4,
        Default_5 = 5,
        Default_6 = 6,
        Default_7 = 7,
        Default_8 = 8,
        Default_9 = 9,
        Temp_0 = 10,
        Telemetry_0 = 11,
        Telemetry_1 = 12,
        Telemetry_2 = 13,
        Telemetry_3 = 14,
        Telemetry_4 = 15,
        Telemetry_5 = 16,
        Telemetry_6 = 17,
        Telemetry_7 = 18,
        Telemetry_8 = 19,
        Telemetry_9 = 20,
        OldFormat_0 = 21,
        Max = 22,
        Default_Head = 0,
        Default_Tail = 9,
        Temp_Head = 10,
        Temp_Tail = 10,
        Telemetry_Head = 11,
        Telemetry_Tail = 20,
    };
}
namespace via::physics {
    enum class CastRayOption : int64_t {
        AllHits = 0,
        DisableBackFacingTriangleHits = 1,
        DisableFrontFacingTriangleHits = 2,
        BackFacingTriangleHits = 3,
        FrontFacingTriangleHits = 4,
        NearSort = 5,
        InsideHits = 6,
        OneHitBreak = 7,
        Max = 8,
    };
}
namespace app::ropeway::MainSceneUpdateManager {
    enum class SetAction : int64_t {
        None = 0,
        FALSE_ = 1,
        TRUE_ = 2,
    };
}
namespace via::wwise::SetState {
    enum class SetTiming : int64_t {
        Start = 0,
        End = 1,
    };
}
namespace via::navigation::NavigationWaypoint {
    enum class RegionShapeType : int64_t {
        OBB = 0,
        Sphere = 1,
    };
}
namespace via::navigation::map::NodeContent {
    enum class EdgeAttribute : int64_t {
        Contour = 1,
        Vertical = 4,
    };
}
namespace app::ropeway::gui::CostumeBehavior {
    enum class CostumeMode : int64_t {
        NORMAL_MODE = 0,
        CLASSIC_MODE = 1,
        CUSTOM_MODE = 2,
    };
}
namespace via::motion {
    enum class TimingType : int64_t {
        Now = 0,
        End = 1,
        SyncPoint = 2,
    };
}
namespace via::render {
    enum class StencilOp : int64_t {
        Keep = 1,
        Zero = 2,
        Replace = 3,
        IncSat = 4,
        DecSat = 5,
        Invert = 6,
        Inc = 7,
        Dec = 8,
        Num = 9,
    };
}
namespace app::ropeway::fsmv2::enemy::em7100 {
    enum class CLING_HP_TAG : int64_t {
        CLING_HP = 3175209577,
    };
}
namespace via::navigation::PathfinderSurface {
    enum class PathCreateType : int64_t {
        UpperOnly = 0,
        LowerOnly = 1,
        Hybrid = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickCustomJackControl::EventDataEnemy {
    enum class SetPosType : int64_t {
        NONE = 0,
        POS = 1,
        AS_CHILD_BY_WORLD = 2,
        AS_CHILD_BY_LOCAL = 3,
        INTERP_NULLOFS_TO_GMK = 4,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class ActionReachKind : int64_t {
        HOLD = 0,
        BITE_DIRECT = 1,
        EAT = 2,
    };
}
namespace via::os {
    enum class SocketError : int64_t {
        Pending = 0xFFFFFFFF,
        None = 0,
        General = 1,
        NoEnoughMemory = 2,
        InvalidArgument = 3,
        NotProvided = 4,
        AlreadyInitialized = 5,
        NotInitialized = 6,
        CreateDescriptor = 7,
        SetOption = 8,
        GetOption = 9,
        Connect = 10,
        Bind = 11,
        Listen = 12,
        Accept = 13,
        SendSelf = 14,
        SendPeer = 15,
        RecvSelf = 16,
        RecvPeer = 17,
        DisconnectByPeer = 18,
        CloseByPeer = 19,
        DnsGeneral = 20,
        DnsNotFound = 21,
        DnsNoResult = 22,
    };
}
namespace app::ropeway::rogue {
    enum class BaseAreaItemType : int64_t {
        Invalid = 0,
        DefaultExp = 1,
        ExpRate = 2,
        CashBox = 3,
        MemberSlot = 4,
        ShopStockLevel = 5,
        TradeCoIN_ = 6,
        CoinTradeRate = 7,
        StorageSlot = 8,
        Accessory = 9,
    };
}
namespace app::ropeway {
    enum class IkCalculateMode : int64_t {
        STOP = 0,
        PERFORMANCE = 1,
        QUALITY = 2,
    };
}
namespace app::ropeway::gimmick::action::AreaAccessSensor {
    enum class ExecTimingType : int64_t {
        IN_OUT = 0,
        ONCE_ON_CONTACT = 1,
        ONCE_ON_SEPARATE = 2,
    };
}
namespace via::effect::script::VolumeDecalContainer {
    enum class Type : int64_t {
        Blood = 0,
        Scar = 1,
        Water = 2,
    };
}
namespace app::ropeway::InputDefine {
    enum class Type : int64_t {
        DOWN = 0,
        ON = 1,
        UP = 2,
    };
}
namespace via::render::layer::DeferredLighting {
    enum class LightingPathDebug : int64_t {
        Albedo = 0,
        Diffuse = 1,
        Specular = 2,
        DirectLight = 3,
        Probe = 4,
        SSAO = 5,
        LocalCubeMap = 6,
        IBL = 7,
        SSR = 8,
        Max = 9,
    };
}
namespace via::fsm::Action {
    enum class SwitchSetting : int64_t {
        Off = 0,
        On = 1,
    };
}
namespace app::ropeway::gimmick::option::ItemIdSettings::ItemId {
    enum class ItemType : int64_t {
        ITEM = 0,
        WEAPON = 1,
    };
}
namespace via::os {
    enum class ReadWriteLockError : int64_t {
        None = 0,
        Failed = 1,
    };
}
namespace System::Reflection {
    enum class FieldAttributes : int64_t {
        FieldAccessMask = 7,
        PrivateScope = 0,
        Private = 1,
        FamANDAssem = 2,
        Assembly = 3,
        Family = 4,
        FamORAssem = 5,
        Public = 6,
        Static = 16,
        InitOnly = 32,
        Literal = 64,
        NotSerialized = 128,
        SpecialName = 512,
        PinvokeImpl = 8192,
        RTSpecialName = 1024,
        HasFieldMarshal = 4096,
        HasDefault = 32768,
        HasFieldRVA = 256,
        ReservedMask = 38144,
    };
}
namespace via::motion::IkSpineConformGround::ManageJoint {
    enum class JOINT_TYPE : int64_t {
        NON = 0,
        START = 1,
        RAY = 2,
        END = 3,
    };
}
namespace via::gui {
    enum class CursorType : int64_t {
        Fix = 0,
        Move = 1,
        NoCursor = 2,
    };
}
namespace app::ropeway::SurvivorDefine::ActionOrder {
    enum class Precede : int64_t {
        INVALID = 0,
        TURN = 1,
        QUICK_TURN = 2,
        ATTACK = 4,
        RELOAD = 8,
        CHANGE_WEAPON = 16,
        SWITCH_SIGHT = 32,
        SWITCH_LIGHT = 64,
        CHANGE_BULLET = 128,
        STEP_UP = 256,
        STEP_DOWN = 512,
        QUICK_TURN_EX = 1024,
    };
}
namespace via::render::ToneMapping {
    enum class TemporalAAAlgorithm : int64_t {
        Legacy = 0,
        Standard = 1,
        StandardV2 = 2,
        Sharp = 3,
    };
}
namespace via::motion::JointRemapValue::RemapValueItem {
    enum class CalculateMode : int64_t {
        Sum = 0,
        Average = 1,
    };
}
namespace via::network::utility::RequestManager {
    enum class Attr : int64_t {
        None = 0,
        NeedAnswer = 1,
        IsReentrant = 2,
    };
}
namespace app::ropeway::SurvivorDefine::Restriction {
    enum class Target : int64_t {
        None = 0,
        NonUpdate = 1,
        Hide = 2,
        DisabledCharacterController = 4,
        DisabledRequestSetCollider = 8,
        DisabledGoundFixer = 16,
        DisabledIkController = 32,
        DisabledRagdoll = 64,
        DisabledPressController = 128,
        FixedPressController = 256,
        Invincible = 512,
        IgnoreBlow = 1024,
        IgnoreGrapple = 2048,
    };
}
namespace via::os::dialog {
    enum class Error : int64_t {
        None = 0,
        InvalidArgument = 1,
        Busy = 2,
    };
}
namespace via::userdata::CompareNode {
    enum class Operation : int64_t {
        Eq = 0,
        Neq = 1,
        Ge = 2,
        Le = 3,
        Gt = 4,
        Lt = 5,
    };
}
namespace via::os::http_client {
    enum class Method : int64_t {
        Get = 0,
        Post = 1,
        Put = 2,
        Delete = 3,
    };
}
namespace app::ropeway::ReNet {
    enum class PostTimingType : int64_t {
        ManualSave = 0,
        AutoSave = 1,
        CheckPoint = 2,
        Quit = 3,
        ClearResult = 4,
        PauseLoad = 5,
        GameOver = 6,
        GameOverContinue = 7,
        GameOverLoad = 8,
        MainMenuFromBonus = 9,
        ExtraClearResult = 10,
        ExtraQuit = 11,
        ExtraGameOver = 12,
        ExtraRetry = 13,
        LostClearResult = 14,
        LostQuit = 15,
        LostGameOver = 16,
        LostRetry = 17,
        OptionSave = 18,
        FirstTitleMenu = 19,
    };
}
namespace app::ropeway::enemy::em7200 {
    enum class RankParamHash : int64_t {
        MOVE_SPEED_RATE = 408292737,
        SIGN_OF_ATK_HOLD_SPEED_RATE = 4078249293,
        DMG_ACID_TIME = 2406071476,
        DMG_FLASH_TIME = 1079981001,
        BACK_EYE_ENABLE = 94937356,
        FIRE_WALL_TIME = 305581750,
        ATK_HOLD_RATE = 1530565235,
        ADD_ATK_RATE_HL = 2050828995,
        ADD_ATK_RATE_HR = 1518359747,
        ADD_ATK_RATE_TURN = 631551570,
        ADD_ATK_RATE_MIDR = 3582474766,
        SIGN_OF_ATK_SPEED_RATE = 117012651,
    };
}
namespace via::clr::VM {
    enum class CounterType : int64_t {
        CycleGC = 0,
        LocalFrameGC = 1,
        GlobalFrameGC = 2,
        LocalObject = 3,
        LocalSize = 4,
        LocalToGlobal = 5,
        CycleGarbage = 6,
        CycleTrace = 7,
        GlobalGarbage = 8,
        Finalize = 9,
        Max = 10,
    };
}
namespace app::ropeway::behaviortree::condition::survivor::npc::NpcBtCondition_CheckTarget {
    enum class CheckTypeKind : int64_t {
        Empty = 0,
        Player = 1,
        Enemy = 2,
    };
}
namespace app::ropeway::CollisionDefine {
    enum class AttackDirType : int64_t {
        Object = 0,
        Owner = 1,
        Radiation = 2,
    };
}
namespace via::render::layer::CubemapCapture {
    enum class BASIS : int64_t {
        SH_ORDER1 = 0,
        SH_ORDER2 = 1,
        FC3_BASIS = 2,
        AMBIENTCUBE_BASIS = 3,
    };
}
namespace app::ropeway::player::tag {
    enum class StateAttribute : int64_t {
        IDLE = 1603047924,
        WALK = 2546078274,
        JOG = 3884239614,
        TURN = 3340187646,
        HOLD = 1751707637,
        FOCUS = 106233786,
        GAZING = 3693037717,
        REACTION = 404851248,
        JACK = 1831514452,
        DEAD = 1973888406,
        SUPPORT = 1435645415,
        KNIFE = 2773707206,
        CREEPING = 3273685968,
        DAMAGE = 1587301165,
        STEP = 3000703046,
        ATTACK = 2612146075,
        HOLD_START = 2548707948,
        RELOAD = 1426028817,
        CHANGE_BULLET = 2876079280,
        CHANGE_WEAPON = 372752890,
        JOG_START = 2789137022,
        SWITCH_LIGHT = 1031087176,
        WALK_END = 902931070,
        JOG_END = 259076557,
        COUGH = 3843926585,
        PUMP_ACTION = 1277488769,
        TOUCH_DOOR = 4287355391,
        WHEEL = 3442724071,
        SPARK_SHOT = 3338892891,
    };
}
namespace via::behavior::EffectEvent {
    enum class LifeState : int64_t {
        Unknown = 0,
        Wait = 1,
        Initialize = 2,
        Appear = 3,
        Keep = 4,
        KeepHold = 5,
        Vanish = 6,
        Terminate = 7,
    };
}
namespace app::ropeway::survivor::fsmv2::action::SurvivorShootAction {
    enum class Timing : int64_t {
        Start = 0,
        Update = 1,
        Sequence = 2,
    };
}
namespace via::os {
    enum class MutexError : int64_t {
        None = 0,
        Failed = 1,
    };
}
namespace app::ropeway::enemy::em4400::Em4400ThinkChild {
    enum class ESCAPE_ACTION_TYPE : int64_t {
        Point = 0,
        RangeAway = 1,
        CounterFly = 2,
    };
}
namespace app::ropeway::effect::script::EffectRecordManager::StampSaveTargetFilterUserData {
    enum class StampSaveType : int64_t {
        Detail = 0,
        Special_Burn = 1,
        Special_Acid = 2,
        Template = 3,
        UnDiscovered = 4,
    };
}
namespace app::ropeway::gui::FileListBehavior {
    enum class ViewMode : int64_t {
        NONE = 0,
        FILE = 1,
        TUTORIAL = 2,
    };
}
namespace via::navigation {
    enum class CostValueType : int64_t {
        Unit = 0,
        Direct = 1,
    };
}
namespace app::ropeway::fsmv2::DispTextMessages {
    enum class Rno : int64_t {
        Check = 0,
        Draw = 1,
        Wait = 2,
    };
}
namespace app::ropeway::Em4000Define {
    enum class SetType : int64_t {
        TypeZombie_00 = 0,
        TypeZombie_01 = 1,
        TypeZombie_02 = 2,
        TypeZombie_Random = 3,
        TypeCity_A = 4,
        TypeCity_B = 5,
    };
}
namespace via::gui::detail {
    enum class LetterAlignmentH : int64_t {
        Left = 0,
        Center = 1,
        Right = 2,
    };
}
namespace via::effect::script::EffectSphereHolder::SphereInfo {
    enum class Status : int64_t {
        Start = 0,
        Update = 1,
        End = 2,
    };
}
namespace app::ropeway::behaviortree::condition::BtCondition_ActionTargetSide {
    enum class SideType : int64_t {
        Left = 0,
        Right = 1,
        Center = 2,
    };
}
namespace app::ropeway::survivor::fsmv2::action::SurvivorOrientAction {
    enum class Target : int64_t {
        World = 0,
        Damage = 1,
    };
}
namespace via::physics::BoundingVolumeHierarchy {
    enum class ErrorCode : int64_t {
        None = 0,
        FunctorAbort = 1,
        Max = 2,
    };
}
namespace via::autoplay::AutoPlay {
    enum class AutoPlayStatus : int64_t {
        Wait = 0,
        PauseConvert = 1,
        PauseLoad = 2,
        Play = 3,
    };
}
namespace app::Collision::CollisionSystem::Material {
    enum class Effect : int64_t {
        EmSoilA = 0,
        EmGravelA = 1,
        EmSloughA = 2,
        EmStoneA = 3,
        EmRubbleA = 4,
        EmWoodA = 5,
        EmWoodB = 6,
        EmIronA = 7,
        EmIronNetA = 8,
        EmGlassPieceA = 9,
        EmClothA = 10,
        EmMeatA = 11,
        EmPaperA = 12,
        EmWaterA = 13,
        EmWaterB = 14,
        EmWaterC = 15,
        EmWaterDeepD = 16,
        EmNoneA = 17,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class SickKind : int64_t {
        FLASH = 0,
        ROLE_BURN = 1,
    };
}
namespace app::ropeway::weapon::shell::ShellDefine {
    enum class ShellValueSelect : int64_t {
        CalcDamager = 0,
        FixValue = 1,
    };
}
namespace via::gui {
    enum class TextAnimationCommand : int64_t {
        Restart = 0,
        Resume = 1,
        Pause = 2,
        End = 3,
    };
}
namespace app::ropeway::weapon::shell::ShellDefine {
    enum class OperationalStatus : int64_t {
        WAIT = 0,
        ACTIVATE = 1,
        OPERATION = 2,
        SUSPEND = 3,
        DEACTIVATE = 4,
    };
}
namespace app::ropeway::TimelineEventStandbyController {
    enum class Phase : int64_t {
        None = 0,
        StartStandbyToNone = 1,
        StandbyToNoneSub = 2,
        StandbyToNoneMaIN_ = 3,
        StartNoneToStandby = 4,
        NoneToStandbySub = 5,
        NoneToStandbyMaIN_ = 6,
        Standby = 7,
        StartActiveToStandby = 8,
        ActiveToStandbyMaIN_ = 9,
        ActiveToStandbySub = 10,
        StartStandbyToActive = 11,
        StandbyToActiveSub = 12,
        StandbyToActiveMaIN_ = 13,
        Active = 14,
    };
}
namespace via::network::Protocol {
    enum class MemberIndex : int64_t {
        None = 0xFFFFFFFF,
        All = -2,
        Other = -3,
        Self = -4,
        Host = -5,
    };
}
namespace via::render {
    enum class TextureAddressMode : int64_t {
        TextureAddress_Wrap = 1,
        TextureAddress_Mirror = 2,
        TextureAddress_Clamp = 3,
        TextureAddress_Border = 4,
        TextureAddress_MirrorOnce = 5,
    };
}
namespace app::ropeway::GameRankSystem {
    enum class RankPointType : int64_t {
        PlAttackHit = 0,
        PlGetItem = 1,
        PlDamage = 2,
        PlRetry = 3,
        FromFsm = 4,
        FromScript = 5,
        FromDirectSet = 6,
        PlUseWeapon = 7,
        PlUseDefenceItem = 8,
        FromDebug = 9,
    };
}
namespace app::ropeway::PosAccessManager::RequestResult {
    enum class ResultType : int64_t {
        UNSET = 0,
        REQUEST = 1,
        ANYONE_USED = 2,
    };
}
namespace app::ropeway::enemy::em6200::fsm::tag {
    enum class StateAttr : int64_t {
        ThrowEnable = 1693310175,
        PushEnable = 2007970753,
        WalkPushAction = 3336304650,
        DoorOpening = 1935418311,
        DoorAccessEnable = 4291428196,
        ToWalkContinue = 2048993541,
        NoCansel = 891747236,
        CrouchEnable = 1228666758,
        IgnoreChaserCancelOnWarp = 975362516,
        WalkFootCheck = 2359255362,
    };
}
namespace app::ropeway::enemy::em5000 {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Press_Survivor = 0,
        Press_Enemy = 1,
        Damage = 2,
        Damage_Weak = 3,
        AimTarget = 4,
        SoundMarker = 5,
        Sensor_Touch = 6,
        Sensor_Gimmick = 7,
        Sensor_Obstacle = 8,
        Marker_Avoid = 9,
        CameraDrawControl = 10,
        Tangle = 11,
        Attack_Hold = 12,
    };
}
namespace via::areamap::test::DotProductItem {
    enum class Axis : int64_t {
        X = 0,
        Y = 1,
        Z = 2,
    };
}
namespace via::hid {
    enum class ExecuteGroup_hdVibration : int64_t {
        ExecuteGroup_00 = 0,
        ExecuteGroup_01 = 1,
    };
}
namespace app::ropeway::enemy::em7200::fsmv2::condition::Em7200FsmCondition_ThinkMode {
    enum class CompareType : int64_t {
        Equal = 0,
        NotEqual = 1,
        Less = 2,
        LessEq = 3,
        Greater = 4,
        GreaterEq = 5,
    };
}
namespace app::ropeway {
    enum class IkWristSolveMode : int64_t {
        FULL = 0,
        ONLY_WRIST = 1,
        ONLY_ATTITUDE = 2,
    };
}
namespace app::ropeway::gui::GimmickNumberLockHothouseGuiBehavior2 {
    enum class GUIStateType : int64_t {
        NONE = 0,
        SCREENSAVER = 1,
        DEFAULT = 2,
        TO_ROOM = 3,
        ROOM = 4,
        OUT_ROOM = 5,
        ROOM_SUCCESS = 6,
        ROOM_FAILURE = 7,
    };
}
namespace app::ropeway::enemy::em6000::BehaviorTreeAction {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_MOVE = 1,
        ACT_PIVOTTURN = 2,
        ACT_THREAT = 3,
        ACT_HIDE = 4,
        ACT_FLOAT_IDLE = 5,
        ACT_THREAT_IDLE = 6,
        ACT_FILL_IN_IDLE = 7,
        ACT_DRINK_SEWAGE = 8,
        ACT_DRINK_SEWAGE_END = 9,
        ACT_LOOK_AROUND = 10,
        ATK_PUNCH = 2000,
        ATK_DASH_PUNCH = 2001,
        ATK_BACK_SLAP = 2002,
        ATK_BACK_SLASH = 2003,
        ATK_NEAR_GRAPPLE = 2004,
        ATK_FAR_GRAPPLE = 2005,
        ATK_RIGHT_HOOK = 2007,
        ATK_VOMIT_CHILDREN = 2008,
        ATK_UPPER = 2009,
        NONE = 0xFFFFFFFF,
    };
}
namespace app::ropeway::light::VolumeOccludeeManager {
    enum class Phase : int64_t {
        Idle = 0,
        StartResetOcclusionCullingCut = 1,
        WaitResetOcclusionCullingCut = 2,
        DelayedResotre = 3,
        DelayedResotreExec = 4,
    };
}
namespace via::gui {
    enum class ColorType : int64_t {
        Fill = 0,
        Vertical = 1,
        Horizontal = 2,
        EachVertex = 3,
    };
}
namespace via::render {
    enum class InputType : int64_t {
        Static = 0,
        Dynamic = 1,
        PreTransformStatic = 2,
        PreTransformDynamic = 3,
        Max = 4,
        Unknown = 0xFFFFFFFF,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0200_MONTAGE_PARTS_FACE : int64_t {
        FACE00 = 0,
        FACE01 = 1,
        FACE02 = 2,
        FACE03 = 3,
        FACE04 = 4,
        FACE70 = 5,
        NONE = 6,
    };
}
namespace app::ropeway::enemy::em0000::Em0000DropParts {
    enum class DropPartsKind : int64_t {
        Disable = 0xFFFFFFFF,
        LowerBody = 0,
        ArmL = 1,
        ArmR = 2,
        LegL = 3,
        LegR = 4,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class SurvivorMotionType : int64_t {
        None = 0,
        Male = 1,
        Male2 = 2,
        Female = 4,
        Female2 = 8,
        ChildMale = 16,
        ChildFemale = 32,
        Female3 = 64,
    };
}
namespace via::gui {
    enum class InputGridType : int64_t {
        None = 0,
        LeftStick = 1,
        RightStick = 2,
        Dpad = 3,
        Max = 4,
    };
}
namespace via::effect::gpgpu::detail {
    enum class VelocityFlags : int64_t {
        None = 0,
        Radial = 1,
        Normal = 2,
        Direction = 3,
        TypeMask = 15,
    };
}
namespace via::hid {
    enum class KeyboardMainKey : int64_t {
        None = 0,
        Back = 8,
        Tab = 9,
        Clear = 12,
        Enter = 13,
        Return = 13,
        Pause = 19,
        Capital = 20,
        Kana = 21,
        Junja = 23,
        Final = 24,
        Hanja = 25,
        Escape = 27,
        Convert = 28,
        NonConvert = 29,
        Accept = 30,
        ModeChange = 31,
        Space = 32,
        Prior = 33,
        Next = 34,
        End = 35,
        Home = 36,
        Left = 37,
        Up = 38,
        Right = 39,
        Down = 40,
        Select = 41,
        Print = 42,
        Execute = 43,
        SnapShot = 44,
        Insert = 45,
        Delete = 46,
        Help = 47,
        Alpha0 = 48,
        Alpha1 = 49,
        Alpha2 = 50,
        Alpha3 = 51,
        Alpha4 = 52,
        Alpha5 = 53,
        Alpha6 = 54,
        Alpha7 = 55,
        Alpha8 = 56,
        Alpha9 = 57,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LWIN_ = 91,
        RWIN_ = 92,
        Apps = 93,
        Sleep = 95,
        NumPad0 = 96,
        NumPad1 = 97,
        NumPad2 = 98,
        NumPad3 = 99,
        NumPad4 = 100,
        NumPad5 = 101,
        NumPad6 = 102,
        NumPad7 = 103,
        NumPad8 = 104,
        NumPad9 = 105,
        Multiply = 106,
        Add = 107,
        Separator = 108,
        Subtract = 109,
        Decimal = 110,
        Divide = 111,
        F1 = 112,
        F2 = 113,
        F3 = 114,
        F4 = 115,
        F5 = 116,
        F6 = 117,
        F7 = 118,
        F8 = 119,
        F9 = 120,
        F10 = 121,
        F11 = 122,
        F12 = 123,
        F13 = 124,
        F14 = 125,
        F15 = 126,
        F16 = 127,
        F17 = 128,
        F18 = 129,
        F19 = 130,
        F20 = 131,
        F21 = 132,
        F22 = 133,
        F23 = 134,
        F24 = 135,
        NumLock = 144,
        Scroll = 145,
        NumPadEnter = 146,
    };
}
namespace app::ropeway::RagdollController {
    enum class RagdollFixStatus : int64_t {
        DYNAMIC = 0,
        FIX = 1,
    };
}
namespace via::gui {
    enum class CircleColorType : int64_t {
        Fill = 0,
        InOut = 1,
    };
}
namespace via::network::wrangler {
    enum class XsapiPropertySet : int64_t {
        Dimensions = 0,
        Measurement = 1,
    };
}
namespace via::network::session {
    enum class FilterAttr : int64_t {
        None = 0,
        FindFull = 1,
        SameRegion = 2,
    };
}
namespace app::ropeway::camera::userdata::PlyaerCameraForceTwirlerUserData::TwirlerSetting {
    enum class DirectionType : int64_t {
        JackTarget = 0,
        Player = 1,
    };
}
namespace via::autoplay::AutoPlay {
    enum class PadEnterType : int64_t {
        Nomal = 0,
        Reverse = 1,
    };
}
namespace app::ropeway::gui::FloorMapSt12Behavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_1_601_0b = 1,
        Room_1_605_0 = 2,
        Room_1_606_0 = 3,
        Room_1_607_0 = 4,
        Room_1_608_0 = 5,
        Room_1_609_0 = 6,
    };
}
namespace app::ropeway::gamemastering::TutorialManager {
    enum class ID : int64_t {
        INVALID = 0,
        TUTORIAL_001 = 1,
        TUTORIAL_002 = 2,
        TUTORIAL_003 = 3,
        TUTORIAL_004 = 4,
        TUTORIAL_005 = 5,
        TUTORIAL_006 = 6,
        TUTORIAL_007 = 7,
        TUTORIAL_008 = 8,
        TUTORIAL_009 = 9,
        TUTORIAL_010 = 10,
        TUTORIAL_011 = 11,
        TUTORIAL_012 = 12,
        TUTORIAL_013 = 13,
        TUTORIAL_014 = 14,
        TUTORIAL_015 = 15,
        TUTORIAL_016 = 16,
        TUTORIAL_017 = 17,
        TUTORIAL_018 = 18,
        TUTORIAL_019 = 19,
        TUTORIAL_020 = 20,
        TUTORIAL_021 = 21,
        TUTORIAL_022 = 22,
        TUTORIAL_023 = 23,
        TUTORIAL_024 = 24,
        TUTORIAL_025 = 25,
        TUTORIAL_026 = 26,
        TUTORIAL_027 = 27,
        TUTORIAL_028 = 28,
        TUTORIAL_029 = 29,
        TUTORIAL_030 = 30,
        TUTORIAL_STOP_001 = 31,
        TUTORIAL_STOP_002 = 32,
        TUTORIAL_STOP_003 = 33,
        TUTORIAL_STOP_004 = 34,
        TUTORIAL_STOP_005 = 35,
        TUTORIAL_STOP_006 = 36,
        TUTORIAL_STOP_007 = 37,
        TUTORIAL_STOP_008 = 38,
        TUTORIAL_STOP_009 = 39,
        TUTORIAL_STOP_010 = 40,
        TUTORIAL_STOP_011 = 41,
        TUTORIAL_STOP_012 = 42,
        TUTORIAL_STOP_013 = 43,
        TUTORIAL_STOP_014 = 44,
        TUTORIAL_STOP_015 = 45,
        TUTORIAL_STOP_016 = 46,
        TUTORIAL_STOP_017 = 47,
        TUTORIAL_STOP_018 = 48,
        TUTORIAL_STOP_019 = 49,
        TUTORIAL_STOP_020 = 50,
        TUTORIAL_STOP_021 = 51,
        TUTORIAL_STOP_022 = 52,
        TUTORIAL_STOP_023 = 53,
        TUTORIAL_STOP_024 = 54,
        TUTORIAL_STOP_025 = 55,
        TUTORIAL_STOP_026 = 56,
        TUTORIAL_STOP_027 = 57,
        TUTORIAL_STOP_028 = 58,
        TUTORIAL_STOP_029 = 59,
        TUTORIAL_STOP_030 = 60,
        MAX = 61,
    };
}
namespace via::render {
    enum class ShallowWaterRenderingPriority : int64_t {
        PreDecal = 0,
        PostDecal = 1,
    };
}
namespace via::render {
    enum class LightImportantLevel : int64_t {
        Highest = 0,
        High = 1,
        Normal = 2,
        Low = 3,
        Lowest = 4,
    };
}
namespace app::ropeway::gimmick::action::TriggerSight {
    enum class TargetPosType : int64_t {
        POSITION = 0,
        ENEMY = 1,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class CreepDetailKindID : int64_t {
        TYPE_A = 0,
        TYPE_B = 1,
        TYPE_B_SYM = 2,
        TYPE_C = 3,
        TYPE_C_SYM = 4,
        TYPE_D = 5,
        TYPE_D_SYM = 6,
        TYPE_E = 7,
        TYPE_E_SYM = 8,
        TYPE_F = 9,
        TYPE_F_SYM = 10,
        TYPE_G = 11,
        TYPE_G_SYM = 12,
        TYPE_H = 13,
        TYPE_I = 14,
        TYPE_J = 15,
    };
}
namespace via::render::detail {
    enum class PrimitiveGUIMaskMode : int64_t {
        None = 0,
        Target = 1,
        TargetReverse = 2,
        Mask = 3,
        MaskBlend2 = 4,
        MaskBlend3 = 5,
    };
}
namespace app::ropeway::enemy::em9000::tracks::Em9000WaitChaseTrack {
    enum class WaitChaseType : int64_t {
        NOWAIT = 0,
        WAIT = 1,
    };
}
namespace via::effect::lensflare {
    enum class AxisType : int64_t {
        PositiveX = 0,
        PositiveY = 1,
        PositiveZ = 2,
        NegativeX = 3,
        NegativeY = 4,
        NegativeZ = 5,
    };
}
namespace app::ropeway::Enemy3DNaviMoveSupporter {
    enum class MoveType : int64_t {
        NONE = 0,
        GROUND_TO_CEILING = 1,
        GROUND_TO_WALL = 2,
        WALL_TO_GROUND = 3,
        WALL_TO_WALL_INSIDE = 4,
        WALL_TO_WALL_OUTSIDE = 5,
        WALL_TO_CEILING = 6,
        CEILING_TO_WALL = 7,
        CEILING_TO_GROUND = 8,
    };
}
namespace app::ropeway::AssetPackageManager {
    enum class OptionType : int64_t {
        AUTO_DOOR = 0,
        RTT = 1,
    };
}
namespace app::ropeway {
    enum class IkComponentType : int64_t {
        Leg = 0,
        NLeg = 1,
        FourLeg = 2,
        Dog = 3,
        Spine = 4,
        Attitude = 5,
        LookAt = 6,
        NboneCCD = 7,
        TwoArm = 8,
        ArmFit = 9,
        Hand = 10,
        HumanRetarget = 11,
        Invalid = 0xFFFFFFFF,
    };
}
namespace app::ropeway::gamemastering::MovieManager {
    enum class PlayingMovie : int64_t {
        TRUE_ = 0,
        FALSE_ = 1,
    };
}
namespace app::ropeway::WwiseTagOnStopTrigger_v2 {
    enum class CastRayDirectionEnum : int64_t {
        Down = 0,
        ObjectDirection = 1,
        ManualDirection = 2,
    };
}
namespace app::ropeway::enemy::em7000::Em7000Define {
    enum class CompareType : int64_t {
        EQ = 0,
        NE = 1,
        GE = 2,
        GT = 3,
        LE = 4,
        LT = 5,
    };
}
namespace app::ropeway::enemy::em7300 {
    enum class RankParamHash : int64_t {
        MOVE_SPEED_RATE = 408292737,
        WALL_FALL_EYE_BREAK_COUNT = 3697269126,
        TRAIN_FALL_EYE_BREAK_COUNT = 626928270,
    };
}
namespace app::ropeway::enemy::em6200::MotionPattern {
    enum class Search : int64_t {
        F = 0,
        B = 1,
        LR = 2,
        Num = 3,
    };
}
namespace app::ropeway::gimmick::option::TimelineDirector {
    enum class ExternalType : int64_t {
        Survivor = 0,
        Enemy = 1,
    };
}
namespace via::motion::tree::BaseGamePadButtonNode {
    enum class BtnKind : int64_t {
        LL = 0,
        LU = 1,
        LR = 2,
        LD = 3,
        RL = 4,
        RU = 5,
        RR = 6,
        RD = 7,
        LB = 8,
        RB = 9,
    };
}
namespace app::ropeway::enemy::em7200::Em7200ThrowBox {
    enum class BoxTypeParam : int64_t {
        BOX = 0,
        CAPSULE = 1,
        LARGE = 2,
    };
}
namespace app::ropeway::leveldesign::ScenarioStandbyController {
    enum class Phase : int64_t {
        None = 0,
        StartStandbyToNone = 1,
        StandbyToNoneSub = 2,
        StandbyToNoneMaIN_ = 3,
        StartNoneToStandby = 4,
        NoneToStandbySub = 5,
        NoneToStandbyMaIN_ = 6,
        Standby = 7,
        StartActiveToStandby = 8,
        ActiveToStandbyMaIN_ = 9,
        ActiveToStandbySub = 10,
        StartStandbyToActive = 11,
        StandbyToActiveSub = 12,
        StandbyToActiveMaIN_ = 13,
        Active = 14,
    };
}
namespace app::ropeway::player::tag {
    enum class ControlAttribute : int64_t {
        DIRECTING = 500911402,
        DISABLE_LIGHT_SWITCH = 1734260481,
        RELOAD = 1584842619,
        INHIBIT_NEXT_LAYER_ACTION = 3253696941,
    };
}
namespace app::ropeway::gui::FloorMapSt5DBehavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        sm40_060_SecurityDoor_U09 = 1,
    };
}
namespace via::effect::detail {
    enum class BlendType : int64_t {
        AlphaBlend = 0,
        Physical = 1,
        AddContrast = 2,
        EdgeBlend = 3,
    };
}
namespace via::motion::JointExMultiRemapValue::OutputData {
    enum class OutputMode : int64_t {
        Mode_Sum = 0,
        Mode_Average = 1,
    };
}
namespace app::ropeway::enemy::em5000::MotionVariation {
    enum class Variation : int64_t {
        VARIATION_0 = 0,
        VARIATION_1 = 1,
    };
}
namespace via::render::LightShaft {
    enum class LightShaftType : int64_t {
        AABB = 0,
        LightSpaceCalc = 1,
        SimpleVolume = 2,
        Num = 3,
    };
}
namespace via::motion::MotionListElementData {
    enum class Flags : int64_t {
        Flags_Mirror = 1,
        Flags_LocalTree = 2,
        Flags_3 = 4,
        Flags_4 = 8,
        Flags_5 = 16,
        Flags_6 = 32,
        Flags_7 = 64,
        Flags_SeqOnly = 128,
    };
}
namespace via::Rect3DXZ {
    enum class VoronoiId : int64_t {
        L_Bit = 1,
        R_Bit = 2,
        T_Bit = 4,
        B_Bit = 8,
        Internal = 0,
        Edge_L_Bit = 1,
        Edge_R_Bit = 2,
        Edge_T_Bit = 4,
        Edge_B_Bit = 8,
        Vertex_LT_Bit = 5,
        Vertex_LB_Bit = 9,
        Vertex_RT_Bit = 6,
        Vertex_RB_Bit = 10,
    };
}
namespace via::os {
    enum class BackgroundInstallSpeed : int64_t {
        Slow = 0,
        Suspend = 1,
        Fast = 2,
    };
}
namespace via::attribute::FsmCategoryAttribute {
    enum class Category : int64_t {
        None = 0,
        Fsm = 1,
        Mot = 2,
        Auto = 4,
        BehaviorTree = 8,
    };
}
namespace via::effect::script::EffectMaterialController {
    enum class PlayTypeEnum : int64_t {
        Return = 0,
        EndKeep = 1,
    };
}
namespace via::Application {
    enum class RuntimeTargetType : int64_t {
        Target_Undefined = 0,
        TargetMachine_Mask = 15,
        TargetMachine_PC = 1,
        TargetMachine_PS4 = 2,
        TargetMachine_XB1 = 3,
        TargetMachine_NSW = 4,
        TargetMachineDetail_Mask = 240,
        TargetMachineDetail_PS4Base = 16,
        TargetMachineDetail_PS4NEO = 32,
        TargetMachineDetail_XB1 = 48,
        TargetMachineDetail_XB1X = 64,
        TargetOS_Mask = 3840,
        TargetOS_Windows = 256,
        TargetOS_PS4 = 512,
        TargetOS_NSW = 768,
        TargetServicePlatform_Mask = 61440,
        TargetServicePlatform_Default = 4096,
        TargetServicePlatform_Steam = 8192,
        TargetServicePlatform_UWP = 12288,
        TargetServicePlatform_WeGame = 16384,
    };
}
namespace app::ropeway::SurvivorColliderTrack {
    enum class ColliderGroup : int64_t {
        INVALID = 0xFFFFFFFF,
        PRESS = 0,
        GIMMICK = 1,
        DAMAGE = 2,
        LOAD_REQUEST = 3,
        TOUCH_APPEAL = 4,
        SCENE_REQUEST = 5,
        SOUND_SENSOR = 6,
        SOUND_MARKER = 7,
        FATAL_BLOW = 8,
    };
}
namespace via::motion::Motion {
    enum class DrawTarget : int64_t {
        Hidden = 0,
        MaIN_ = 1,
        All = 2,
    };
}
namespace via::wwise {
    enum class WwiseProjectNo : int64_t {
        WwiseProjectNo_0 = 0,
        WwiseProjectNo_1 = 1,
        WwiseProjectNo_2 = 2,
        WwiseProjectNo_3 = 3,
        WwiseProjectNo_Default = 0,
    };
}
namespace app::ropeway::ChoreographPlayer {
    enum class ChoreographType : int64_t {
        NONE = 0,
        MOTION = 1,
        MOTIONS = 2,
        FSM = 3,
        MOTION_FSM = 4,
        MAT_EMISSIVE = 5,
        MAT_COLOR = 6,
        SOUND = 7,
        MOVE_POS = 8,
        PRM_CRV_ANIM = 9,
        WAIT_TIME = 10,
        COLLIDERS = 11,
        DISP_MESSAGES = 12,
        DISP_MESH_PARTS = 13,
        TIMELINE = 14,
    };
}
namespace app::ropeway::movie::MoviePlayer {
    enum class Phase : int64_t {
        Wait = 0,
        Releasing = 1,
        PrefabRelease = 2,
        PrefabStandby = 3,
        Instantiating = 4,
        Ready = 5,
        Kick = 6,
        Request = 7,
        Play = 8,
        Pause = 9,
        Resume = 10,
        Refresh = 11,
        Finish = 12,
    };
}
namespace app::ropeway::ExtraContentsManager {
    enum class ProcessType : int64_t {
        None = 0,
        WaitMounted = 1,
        Check = 2,
    };
}
namespace app::ropeway::camera::CameraDefine {
    enum class Layer : int64_t {
        CUT_SCENE = 0,
        GIMMICK = 1,
        GIMMICK_ADD = 2,
        ACTION = 3,
    };
}
namespace app::ropeway::fsmv2::PlayAnimationWithJointController {
    enum class TargetType : int64_t {
        TARGET_REF = 0,
        SELF = 1,
        PARENT = 2,
    };
}
namespace app::ropeway::gui::ZombieFinishIconBehavior {
    enum class State : int64_t {
        NONE = 0,
        TO_ON = 1,
        DRAWING = 2,
        TO_OFF = 3,
    };
}
namespace via::effect::script::EffectTimelineChecker {
    enum class Routine : int64_t {
        Initialize = 0,
        Check = 1,
        Finish = 2,
        WaitEnd = 3,
    };
}
namespace app::ropeway::implement::userdata::ImplementUserData::AttachSetting {
    enum class InspectorGroup : int64_t {
        Equiped = 0,
        Holster = 1,
    };
}
namespace via::motion::detail::ChainNode {
    enum class HitFlags : int64_t {
        None = 0,
        Self = 2,
        Model = 4,
        Collider = 8,
        Angle = 16,
        Group = 32,
        VGround = 64,
        Collision = 110,
    };
}
namespace app::ropeway::TerrainAnalyzer {
    enum class StepType : int64_t {
        NONE = 0,
        ASCEND = 1,
        DESCEND = 2,
    };
}
namespace via::effect::script::EPVExpertFootLandingData {
    enum class FootLandingRelationType : int64_t {
        Defalut = 0,
        FollowParent = 1,
        InitializationParent = 2,
    };
}
namespace app::ropeway::gui::FloorMapSt3DBehavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_3_616_0c = 1,
        Room_3_617_0a = 2,
        Room_3_617_0b = 3,
        Room_3_621_0 = 4,
        Room_3_623_0a = 5,
        Room_3_623_0b = 6,
        Room_3_624_0 = 7,
        Room_3_625_0 = 8,
        Room_3_627_0a = 9,
        Room_3_627_0b = 10,
    };
}
namespace app::ropeway::weapon::shell::ShellManager {
    enum class BombAttackPriority : int64_t {
        Explosion = 1,
        Blow = 2,
        ShockWave = 3,
    };
}
namespace via::wwise::midi {
    enum class MMC : int64_t {
        Stop = 1,
        Play = 2,
        RecStrobe = 3,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class ENEMY_PLANE_ANGLE : int64_t {
        WALL = 70,
        CEILING = 110,
    };
}
namespace app::ropeway::GimmickOptionMotionCameraSettings {
    enum class ConstType : int64_t {
        SELF = 0,
        TARGET = 1,
        NONE = 2,
    };
}
namespace app::ropeway::gimmick::option::PlayerConditionCheckSettings::Param::CheckActionParam {
    enum class StatusType : int64_t {
        Walk = 0,
        Jog = 1,
        Hold = 2,
        Attack = 3,
        Light = 4,
    };
}
namespace app::ropeway::EnvironmentStandbyController {
    enum class FolderActivateReason : int64_t {
        None = 0,
        SceneActivate = 1,
        DynamicSpaceConnection = 4,
        LinkDraw = 8,
        EventSpecialControl = 65536,
        NormalControlMask = 13,
    };
}
namespace app::ropeway::fsmv2::TraceVariableAction {
    enum class TimingType : int64_t {
        Start = 0,
        Update = 1,
        End = 2,
    };
}
namespace via::str::detail::format::csharp::format_item {
    enum class ExFmtFlag : int64_t {
        Ignore = 0,
        SetPrecision = 0,
        ZeroPadding = 0,
        Centering = 0,
    };
}
namespace via::render::LDRImagePlane {
    enum class BlendType : int64_t {
        Overlay = 0,
        Max = 1,
    };
}
namespace app::ropeway::gui::FloorMapSt3BBehavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        Door_5_3_005 = 1,
        Door_6_1_006 = 2,
        Door_6_1_011 = 3,
        Door_6_1_012 = 4,
        Door_6_1_013 = 5,
        Door_6_1_023 = 6,
        Door_6_1_027 = 7,
        Door_6_1_028 = 8,
        Door_6_1_029 = 9,
        Door_6_1_030 = 10,
        Door_6_1_032 = 11,
    };
}
namespace app::ropeway::gui::GimmickHallPcGuiBehavior {
    enum class StateType : int64_t {
        NONE = 0,
        WARNING = 1,
        MAP = 2,
        CAMERA = 3,
        CAMERA2 = 4,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::Rotate {
    enum class DirectionType : int64_t {
        Target = 0,
        Local = 1,
        World = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickJointController {
    enum class TargetParam : int64_t {
        Position = 0,
        Degree = 1,
        Scale = 2,
    };
}
namespace via::userdata {
    enum class ParamType : int64_t {
        Unknown = 0,
        Bool = 1,
        U8 = 2,
        S8 = 3,
        U16 = 4,
        S16 = 5,
        S32 = 6,
        U32 = 7,
        S64 = 8,
        U64 = 9,
        F32 = 10,
        F64 = 11,
        Str8 = 12,
        Str16 = 13,
        Address = 14,
        Object = 15,
        Vec2 = 16,
        Vec3 = 17,
        Vec4 = 18,
        Matrix = 19,
        Guid = 20,
    };
}
namespace via::graph {
    enum class VertexIndex : int64_t {
        Invalid = 0xFFFFFFFF,
        From = 0,
        To = 1,
    };
}
namespace app::ropeway {
    enum class ActiveUserPadPairingResult : int64_t {
        OK = 0,
        NecessaryAccountPicker = 1,
        NecessaryLastInputDevice = 2,
    };
}
namespace via::relib::effect::EffectControlAction {
    enum class TargetModeEnum : int64_t {
        TargetOnly = 0,
        TargetAndChildren = 1,
        ChildrenOnly = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickDoorBase {
    enum class PeekedRoutine : int64_t {
        INIT = 0,
        WAIT = 1,
        OPEN = 2,
        CLOSE = 3,
    };
}
namespace via::motion::detail::ChainGroupData {
    enum class AttrFlags : int64_t {
        None = 0,
        RootRotation = 1,
        AngleLimit = 2,
        ExtraNode = 4,
        CollisionDefault = 8,
        CollisionSelf = 16,
        CollisionModel = 32,
        CollisionVGround = 64,
        CollisionCollider = 128,
        CollisionGroup = 256,
        EnablePartBlend = 512,
        WindDefault = 1024,
        TransAnimation = 2048,
        AngleLimitRestitution = 4096,
    };
}
namespace via::gui::detail {
    enum class PageAlignmentV : int64_t {
        Top = 0,
        Center = 1,
        Bottom = 2,
    };
}
namespace app::ropeway::gimmick::option::GimmickPlugPlaceSettings {
    enum class PlugType : int64_t {
        None = 0,
        Bishop = 1,
        Rook = 2,
        King = 3,
        Queen = 4,
        Knight = 5,
        Pawn = 6,
    };
}
namespace via::hid::mouse {
    enum class ManipulatorClientType : int64_t {
        NULL_ = 0,
        DirectInput = 1,
        RawInput = 2,
        WindowMessage = 3,
        GlobalParameter = 4,
        RuntimeDefault = 5,
        ToolDefault = 6,
    };
}
namespace via::motion::IkMultipleDamageAction {
    enum class CalcuCenterOffset : int64_t {
        ImpactState = 0,
        MaxState = 1,
    };
}
namespace app::ropeway::enemy::userdata::Em7100PillerAttackUserData {
    enum class PillerAttackType : int64_t {
        HoldCraw = 0,
        BreakPiller = 1,
    };
}
namespace app::ropeway::enemy::em4400::Em4400ThinkChild {
    enum class MOVE_ACTION_TYPE : int64_t {
        Line = 0,
        Wave = 1,
        Zigzag = 2,
        Circle = 3,
        NUM = 4,
    };
}
namespace app::ropeway::gamemastering::MainFlowManager {
    enum class LostDifficulty : int64_t {
        NORMAL = 0,
        TRAINING = 1,
        MAX = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickBreakable {
    enum class BreakableState : int64_t {
        Init = 0,
        Wait = 1,
        Broken = 2,
    };
}
namespace app::ropeway::camera {
    enum class CameraTwirlStatusType : int64_t {
        CONSTANT = 0,
        ACCELARATE = 1,
    };
}
namespace app::ropeway::enemy::em6000::fsmv2::action::Em6000FsmAction_SetMotionBlendRate {
    enum class BLEND_TYPE : int64_t {
        VomitInit = 0,
        Vomit = 1,
    };
}
namespace via::network::protocol {
    enum class MemberIndex : int64_t {
        None = 0xFFFFFFFF,
        All = -2,
        Other = -3,
        Self = -4,
        Host = -5,
    };
}
namespace via::effect::detail {
    enum class FlipType : int64_t {
        None = 0,
        Flip = 1,
        RandomFlip = 2,
    };
}
namespace app::ropeway::effect::script::VFXJointManager {
    enum class TemplateEmitVec : int64_t {
        X_Plus = 0,
        X_Minus = 1,
        Y_Plus = 2,
        Y_Minus = 3,
        Z_Plus = 4,
        Z_Minus = 5,
    };
}
namespace app::ropeway::enemy::em5000 {
    enum class RankParamHash : int64_t {
        MOVE_SPEED = 3791250704,
        WALK_TOTTER = 3613919294,
        HOLD_RANGE = 2289916401,
        HOLD_RANGE_IDLE = 3584082088,
        HOLD_RANGE_BURNUP = 840914917,
        WEAK_PARTS_REVIVAL_TIME = 1553321746,
        WAKEUP_TIME = 4254072029,
    };
}
namespace via::render::LightProbes {
    enum class LightProbesPriority : int64_t {
        Base = 0,
        Low = 1,
        Middle = 2,
        High = 3,
        Higher = 4,
        Highest = 5,
    };
}
namespace app::ropeway::survivor::npc::NpcOrderDefine {
    enum class MoveCommandKind : int64_t {
        SetTransform = 0,
        MoveToPoint = 1,
        LineTraceMove = 2,
        Invalid = 3,
    };
}
namespace via {
    enum class ScreenShotImageSize : int64_t {
        ScreenShotImageSize_1280x720 = 0,
    };
}
namespace via::render {
    enum class Blend : int64_t {
        Zero = 1,
        One = 2,
        SrcColor = 3,
        InvSrcColor = 4,
        SrcAlpha = 5,
        InvSrcAlpha = 6,
        DestAlpha = 7,
        InvDestAlpha = 8,
        DestColor = 9,
        InvDestColor = 10,
        SrcAlphaSat = 11,
        BlendFactor = 12,
        InvBlendFactor = 13,
        Src1Color = 14,
        InvSrc1Color = 15,
        Src1Alpha = 16,
        InvSrc1Alpha = 17,
        Num = 18,
    };
}
namespace app::ropeway::BlastBoilerAttackUserDataToEnemy {
    enum class BlastBoilerType : int64_t {
        Normal = 0,
        Special = 1,
    };
}
namespace app::ropeway::OperatorDefine::LogicalOperator {
    enum class Type : int64_t {
        AND = 0,
        OR = 1,
    };
}
namespace via::attribute::RemoteProperty {
    enum class SyncMode : int64_t {
        Copy = 0,
        Kill = 1,
        Transaction = 2,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class BodyParts : int64_t {
        Body = 0,
        Sling = 100,
        Handgun = 200,
    };
}
namespace app::ropeway::gimmick::action::GimmickRogueRaccoonFigure {
    enum class State : int64_t {
        INVALID = 0,
        ALIVE_WAIT = 1,
        ALIVE_DANCING = 2,
        ALIVE_LOST_PL = 3,
        ALIVE_NOPOWER = 4,
        BROKEN = 5,
    };
}
namespace app::ropeway::NpcDefine {
    enum class ThinkMode : int64_t {
        IDLE = 0,
        FOLLOW_PLAYER = 1,
        MOVE_TO_POINT = 2,
    };
}
namespace app::ropeway::enemy::em7200::Em7200Think {
    enum class CheckID : int64_t {
        ATK_CONTINUOUS = 0,
        ATK_CONTINUOUS_ANGRY = 1,
        ATK_SHORT_MID_L = 2,
        ATK_BACK_MID_L = 3,
        ATK_HOLD_START = 4,
        ACT_BACKSTEP = 5,
        ATK_DASH = 6,
        ATK_TURN180 = 7,
        ATK_360 = 8,
        ATK_RAGE = 9,
        ATK_SHORT_HIGH_L = 10,
        ATK_SHORT_HIGH_R = 11,
        ATK_ZERO_MID_R = 12,
        ACT_WALK_APPROACH_F = 13,
        NoSet = 14,
    };
}
namespace via::render {
    enum class DsvFlag : int64_t {
        Dsv_ReadOnlyDepth = 1,
        Dsv_ReadOnlyStencil = 2,
        Dsv_StencilLeft = 4,
        Dsv_StencilRight = 8,
    };
}
namespace app::ropeway::gimmick::action::GimmickCustomJackControl::EventDataSurvivor {
    enum class SetPosType : int64_t {
        INTERP_POS = 0,
        INTERP_ROT = 1,
        INTERP_POSROT = 2,
        INTERP_BY_NULLOFS = 3,
        INTERP_BY_NULLOFS_TO_GMK = 4,
        NONE = 5,
    };
}
namespace via::motion::IkMultipleDamageAction {
    enum class CalculationBendRotation : int64_t {
        Parent = 0,
        Root = 1,
    };
}
namespace app::ropeway::enemy::userdata::Em7000DamageUserData {
    enum class RegionType : int64_t {
        Head = 0,
        Body = 1,
        LegR = 2,
        LegL = 3,
        EyeBall = 4,
        ArmR = 5,
        ArmL = 6,
    };
}
namespace app::ropeway::weapon::shell::ShellBase {
    enum class ThrowShellStatusInformation : int64_t {
        NoSuspend = 0,
        Update = 1,
        LateUpdate = 2,
    };
}
namespace via::effect::script::EPVDataBase {
    enum class RotateBase : int64_t {
        ParentJoint = 0,
        ModelNULL_ = 1,
    };
}
namespace app::ropeway::timeline::TimelineEventSettingTable::FlowSetting::PillarBoxUIParam {
    enum class DispType : int64_t {
        Default = 0,
        FadeOutOnly = 1,
        FadeInOnly = 2,
        FadeOutFadeIN_ = 3,
    };
}
namespace app::ropeway {
    enum class FourFootLockBitFlag : int64_t {
        LEFT = 1,
        RIGHT = 2,
        FRONT_LEFT = 4,
        FRONT_RIGHT = 8,
    };
}
namespace via::render {
    enum class InterporateResolve : int64_t {
        Default = 0,
        Fast = 1,
    };
}
namespace via {
    enum class ModuleEntry : int64_t {
        Initialize = 0,
        InitializeStorage = 1,
        InitializeResourceManager = 2,
        InitializeScene = 3,
        InitializeRemoteHost = 4,
        InitializeVM = 5,
        InitializeSystemService = 6,
        InitializeDialog = 7,
        InitializeShareService = 8,
        InitializeUserService = 9,
        InitializeGlobalUserData = 10,
        InitializeSteam = 11,
        InitializeWeGame = 12,
        InitializeRenderer = 13,
        InitializeHID = 14,
        InitializeEffect = 15,
        InitializeGeometry = 16,
        InitializeWwise = 17,
        InitializeGUI = 18,
        InitializeMotion = 19,
        InitializeBehaviorTree = 20,
        InitializeAutoPlay = 21,
        InitializeScenario = 22,
        InitializeAreaMap = 23,
        InitializeFSM = 24,
        InitializeNavigation = 25,
        InitializeTimeline = 26,
        InitializePhysics = 27,
        InitializeDynamics = 28,
        InitializeHavok = 29,
        InitializeBake = 30,
        InitializeNetwork = 31,
        InitializePuppet = 32,
        InitializeVoiceChat = 33,
        InitializeStore = 34,
        InitializeBrowser = 35,
        InitializeDevelopSystem = 36,
        InitializeBehavior = 37,
        InitializeMovie = 38,
        InitializeSkuService = 39,
        InitializeTelemetry = 40,
        InitializeNNFC = 41,
        InitializeThreadPool = 42,
        Setup = 43,
        SetupResourceManager = 44,
        SetupStorage = 45,
        SetupGlobalUserData = 46,
        SetupScene = 47,
        SetupDevelopSystem = 48,
        SetupUserService = 49,
        SetupSystemService = 50,
        SetupShareService = 51,
        SetupVM = 52,
        SetupHID = 53,
        SetupRenderer = 54,
        SetupEffect = 55,
        SetupGeometry = 56,
        SetupWwise = 57,
        SetupMotion = 58,
        SetupNavigation = 59,
        SetupPhysics = 60,
        SetupDynamics = 61,
        SetupHavok = 62,
        SetupMovie = 63,
        SetupNetwork = 64,
        SetupPuppet = 65,
        SetupStore = 66,
        SetupBrowser = 67,
        SetupVoiceChat = 68,
        SetupSkuService = 69,
        SetupTelemetry = 70,
        StartApp = 71,
        SetupAreaMap = 72,
        SetupBehaviorTree = 73,
        SetupFSM = 74,
        SetupGUI = 75,
        SetupNNFC = 76,
        Start = 77,
        StartStorage = 78,
        StartGlobalUserData = 79,
        StartPhysics = 80,
        StartDynamics = 81,
        StartGUI = 82,
        StartTimeline = 83,
        StartAreaMap = 84,
        StartBehaviorTree = 85,
        StartFSM = 86,
        StartWwise = 87,
        StartScene = 88,
        StartNetwork = 89,
        Update = 90,
        UpdateDialog = 91,
        UpdateRemoteHost = 92,
        UpdateStorage = 93,
        UpdateScene = 94,
        UpdateDevelopSystem = 95,
        UpdateWidget = 96,
        UpdateAutoPlay = 97,
        UpdateScenario = 98,
        UpdateCapture = 99,
        BeginFrameRendering = 100,
        UpdateHID = 101,
        UpdateMotionFrame = 102,
        BeginDynamics = 103,
        PreupdateGUI = 104,
        BeginHavok = 105,
        UpdateAIMap = 106,
        CreatePreupdateGroupFSM = 107,
        UpdateGlobalUserData = 108,
        UpdateUserService = 109,
        UpdateSystemService = 110,
        UpdateShareService = 111,
        UpdateSteam = 112,
        UpdateWeGame = 113,
        UpdateNNFC = 114,
        BeginPhysics = 115,
        BeginUpdatePrimitive = 116,
        UpdatePuppet = 117,
        UpdateGUI = 118,
        PreupdateBehavior = 119,
        PreupdateBehaviorTree = 120,
        PreupdateFSM = 121,
        PreupdateTimeline = 122,
        UpdateBehavior = 123,
        CreateNavigationChaIN_ = 124,
        CreateUpdateGroupFSM = 125,
        UpdateTimeline = 126,
        UpdateAreaMap = 127,
        UpdateBehaviorTree = 128,
        UpdateNavigationPrev = 129,
        UpdateFSM = 130,
        UpdateMotion = 131,
        EffectCollisionLimit = 132,
        UpdatePhysicsAfterUpdatePhase = 133,
        UpdateGeometry = 134,
        UpdatePhysicsCharacterController = 135,
        BeginUpdateHavok2 = 136,
        UpdateDynamics = 137,
        UpdateNavigation = 138,
        UpdateConstraintsBegIN_ = 139,
        LateUpdateBehavior = 140,
        EditUpdateBehavior = 141,
        BeginUpdateHavok = 142,
        BeginUpdateEffect = 143,
        UpdateConstraintsEnd = 144,
        UpdatePhysicsAfterLateUpdatePhase = 145,
        PrerenderGUI = 146,
        PrepareRendering = 147,
        UpdateWwise = 148,
        CreateSelectorGroupFSM = 149,
        UpdateNetwork = 150,
        UpdateHavok = 151,
        EndUpdateHavok = 152,
        UpdateFSMSelector = 153,
        BeforeLockSceneRendering = 154,
        EndUpdateHavok2 = 155,
        UpdateJointExpression = 156,
        UpdateBehaviorTreeSelector = 157,
        UpdateEffect = 158,
        EndUpdateEffect = 159,
        UpdateWidgetDynamics = 160,
        LockScene = 161,
        EndDynamics = 162,
        EndPhysics = 163,
        WaitRendering = 164,
        BeginRendering = 165,
        BeginRenderingDynamics = 166,
        RenderGUI = 167,
        RenderGeometry = 168,
        UpdatePrimitive = 169,
        EndUpdatePrimitive = 170,
        GUIPostPrimitiveRender = 171,
        ShapeRenderer = 172,
        UpdateMovie = 173,
        UpdateTelemetry = 174,
        DrawWidget = 175,
        DevelopRenderer = 176,
        EndRenderingDynamics = 177,
        EndRendering = 178,
        UpdateStore = 179,
        UpdateBrowser = 180,
        UpdateVoiceChat = 181,
        UnlockScene = 182,
        UpdateVM = 183,
        StepVisualDebugger = 184,
        WaitForVblank = 185,
        Terminate = 186,
        TerminateScene = 187,
        TerminateRemoteHost = 188,
        TerminateTelemetry = 189,
        TerminateMovie = 190,
        TerminateWwise = 191,
        TerminateVoiceChat = 192,
        TerminatePuppet = 193,
        TerminateNetwork = 194,
        TerminateStore = 195,
        TerminateBrowser = 196,
        TerminateGUI = 197,
        TerminateAreaMap = 198,
        TerminateBehaviorTree = 199,
        TerminateFSM = 200,
        TerminateNavigation = 201,
        TerminateEffect = 202,
        TerminateGeometry = 203,
        TerminateRenderer = 204,
        TerminateHID = 205,
        TerminateDynamics = 206,
        TerminatePhysics = 207,
        TerminateResourceManager = 208,
        TerminateHavok = 209,
        TerminateShareService = 210,
        TerminateGlobalUserData = 211,
        TerminateStorage = 212,
        TerminateVM = 213,
        Finalize = 214,
        FinalizeThreadPool = 215,
        FinalizeTelemetry = 216,
        FinalizeMovie = 217,
        FinalizeBehavior = 218,
        FinalizeDevelopSystem = 219,
        FinalizeTimeline = 220,
        FinalizePuppet = 221,
        FinalizeNetwork = 222,
        FinalizeStore = 223,
        FinalizeBrowser = 224,
        finalizeAutoPlay = 225,
        finalizeScenario = 226,
        FinalizeBehaviorTree = 227,
        FinalizeFSM = 228,
        FinalizeNavigation = 229,
        FinalizeAreaMap = 230,
        FinalizeMotion = 231,
        FinalizeDynamics = 232,
        FinalizePhysics = 233,
        FinalizeHavok = 234,
        FinalizeBake = 235,
        FinalizeGUI = 236,
        FinalizeWwise = 237,
        FinalizeEffect = 238,
        FinalizeGeometry = 239,
        FinalizeRenderer = 240,
        FinalizeHID = 241,
        FinalizeWeGame = 242,
        FinalizeSteam = 243,
        FinalizeNNFC = 244,
        FinalizeGlobalUserData = 245,
        FinalizeSkuService = 246,
        FinalizeUserService = 247,
        FinalizeShareService = 248,
        FinalizeSystemService = 249,
        FinalizeScene = 250,
        FinalizeVM = 251,
        FinalizeResourceManager = 252,
        FinalizeRemoteHost = 253,
        FinalizeStorage = 254,
        FinalizeDialog = 255,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0100_MONTAGE_PARTS_SHIRT : int64_t {
        NONE = 0,
        SHIRT00 = 1,
        SHIRT00_00_00 = 2,
        SHIRT00_00_01 = 3,
        SHIRT01 = 4,
        SHIRT02 = 5,
        SHIRT02_00_00 = 6,
        SHIRT02_01_00 = 7,
        SHIRT02_02_00 = 8,
        SHIRT02_03_00 = 9,
        SHIRT02_04_00 = 10,
        SHIRT03 = 11,
        SHIRT03_00_00 = 12,
        SHIRT03_01_00 = 13,
        SHIRT03_02_00 = 14,
        SHIRT03_03_00 = 15,
        SHIRT03_04_00 = 16,
        SHIRT04 = 17,
        SHIRT05 = 18,
    };
}
namespace app::ropeway::rogue::RogueDefine {
    enum class RogueMapID : int64_t {
        Invalid = 0,
        st2_201_0 = 28,
        st4_101_0 = 112,
        st4_102_0 = 113,
        st4_103_0 = 114,
        st4_201_0 = 219,
        st4_202_0 = 220,
        st4_203_0 = 221,
        st4_204_0 = 222,
        st4_205_0 = 223,
        st4_206_0 = 224,
        st4_207_0 = 118,
        st4_208_0 = 119,
        st4_209_0 = 120,
        st4_210_0 = 121,
        st4_211_0 = 122,
        st4_212_0 = 123,
        st4_214_0 = 125,
        st4_215_0 = 225,
        st4_216_0 = 226,
        st4_217_0 = 431,
        st4_301_0 = 227,
        st4_303_0 = 229,
        st4_304_0 = 230,
        st4_305_0 = 231,
        st4_306_0 = 232,
        st4_307_0 = 233,
        st4_308_0 = 234,
        st4_309_0 = 235,
        st4_310_0 = 236,
        st4_311_0 = 237,
        st4_312_0 = 238,
        st4_313_0 = 239,
        st4_314_0 = 240,
        st4_402_0 = 243,
        st4_403_0 = 244,
        st4_404_0 = 245,
        st4_407_0 = 248,
        st4_408_0 = 249,
        st4_409_0 = 250,
        st4_411_0 = 252,
        st4_412_0 = 253,
        st4_501_0 = 254,
        st4_502_0 = 255,
        st4_503_0 = 256,
        st4_504_0 = 257,
        st4_505_0 = 258,
        st4_506_0 = 259,
        st4_507_0 = 260,
        st4_508_0 = 261,
        st4_601_0 = 262,
        st4_602_0 = 263,
        st4_603_0 = 264,
        st4_604_0 = 265,
        st4_607_0 = 268,
        st4_608_0 = 269,
        st4_609_0 = 270,
        st4_610_0 = 271,
        st4_701_0 = 272,
        st4_702_0 = 273,
        st4_703_0 = 274,
        st4_704_0 = 275,
        st4_708_0 = 277,
        st4_710_0 = 279,
        st4_713_0 = 409,
        st4_714_0 = 282,
        st4_717_0 = 350,
        st4_750_0 = 352,
        st4_751_0 = 353,
        st4_754_0 = 356,
    };
}
namespace via::Stream {
    enum class Attribute : int64_t {
        Default = 0,
        Write = 1,
        Expandable = 2,
        HasUserBit = 4,
        TypeBitMask = 255,
        UserDataBitMask = 16711680,
    };
}
namespace app::ropeway::gui::ShortcutBehavior {
    enum class Icon : int64_t {
        ICON_NONE = 0,
        ICON_UP = 1,
        ICON_DOWN = 2,
        ICON_LEFT = 3,
        ICON_RIGHT = 4,
    };
}
namespace via::render {
    enum class WindowMode : int64_t {
        Normal = 0,
        FullScreen = 1,
        Borderless = 2,
    };
}
namespace via::motion::script::FootEffectController {
    enum class SETriggerTypeEnum : int64_t {
        FOOT_CONTACT_L = 0,
        FOOT_CONTACT_R = 1,
        FOOT_SLIDE_L = 2,
        FOOT_SLIDE_R = 3,
        FOOT_STEP_L = 4,
        FOOT_STEP_R = 5,
        HAND_CONTACT_L = 6,
        HAND_CONTACT_R = 7,
        NUM = 8,
    };
}
namespace app::ropeway::enemy::em8200::Em8200Bomb {
    enum class State : int64_t {
        Wait = 0,
        CountDown = 1,
        Exploded = 2,
    };
}
namespace app::ropeway::SurvivorMotionKind {
    enum class ModeType : int64_t {
        Normal = 0,
        Combat = 1,
        Tension = 2,
        Water = 3,
        Light = 4,
    };
}
namespace via::Cylinder {
    enum class NGLevel : int64_t {
        None = 0,
        Point = 1,
        SizeNone = 2,
        NanFinite = 3,
        Assert = 4,
        Default = 3,
    };
}
namespace app::ropeway::CameraTwirler {
    enum class Mode : int64_t {
        CONSTANT = 0,
        ACCELARATION = 1,
        SWITCH = 2,
    };
}
namespace app::ropeway::rogue::RogueDefine {
    enum class LotteryResult : int64_t {
        None = 0xFFFFFFFF,
        Medal = 0,
        Weapon = 1,
        RpdCoIN_ = 2,
        Heal = 3,
        Consume = 4,
        Blank = 5,
    };
}
namespace app::ropeway::AimForbidUserData {
    enum class VolumeKind : int64_t {
        Inside = 0,
        Outside = 1,
    };
}
namespace app::ropeway {
    enum class TwoHandLockBitFlag : int64_t {
        LEFT = 1,
        RIGHT = 2,
    };
}
namespace app::ropeway::gamemastering::RogueRecordManager {
    enum class RogueRewardId : int64_t {
        ACCESSORY_00 = 0,
        ACCESSORY_01 = 1,
        ACCESSORY_02 = 2,
        ACCESSORY_03 = 3,
        ACCESSORY_04 = 4,
        ACCESSORY_05 = 5,
        ACCESSORY_06 = 6,
        ACCESSORY_07 = 7,
        ACCESSORY_08 = 8,
        ACCESSORY_09 = 9,
        ACCESSORY_10 = 10,
        ACCESSORY_11 = 11,
        ACCESSORY_12 = 12,
        ACCESSORY_13 = 13,
        ACCESSORY_14 = 14,
        MAX = 15,
        INVALID = 16,
    };
}
namespace via::motion::Chassis {
    enum class State : int64_t {
        Initialized = 0,
        Max = 1,
    };
}
namespace via::hid::virtualKeyboard::nsw {
    enum class TextCheckResult : int64_t {
        Success = 0,
        ShowFailureDialog = 1,
        ShowConfirmDialog = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickAutoDoor {
    enum class DoorShapeType : int64_t {
        SINGLE = 0,
        DOUBLE = 1,
    };
}
namespace via::render::layer::Overlay {
    enum class UITargetType : int64_t {
        Default = 0,
        Max = 1,
    };
}
namespace app::ropeway {
    enum class AttachGroup : int64_t {
        Group_00 = 1,
        Group_01 = 2,
        Group_02 = 4,
        Group_03 = 8,
        Group_04 = 16,
        Group_05 = 32,
        Group_06 = 64,
        Group_07 = 128,
        Group_08 = 256,
        Group_09 = 512,
        Group_10 = 1024,
        Group_11 = 2048,
        Group_12 = 4096,
        Group_13 = 8192,
        Group_14 = 16384,
        Group_15 = 32768,
        Group_16 = 65536,
        Group_17 = 131072,
        Group_18 = 262144,
        Group_19 = 524288,
        Group_20 = 1048576,
        Group_21 = 2097152,
        Group_22 = 4194304,
        Group_23 = 8388608,
        Group_24 = 16777216,
        Group_25 = 33554432,
        Group_26 = 67108864,
        Group_27 = 134217728,
        Group_28 = 268435456,
        Group_29 = 536870912,
        Group_30 = 1073741824,
        Group_31 = 2147483648,
    };
}
namespace via::reflection::detail {
    enum class Stage : int64_t {
        BSS = 0,
        Heap = 1,
    };
}
namespace via::render::layer {
    enum class GBufferLayout : int64_t {
        PreLighting = 0,
        BaseColorMetallicTranslucency = 1,
        NormalXNormalYRoughnessMisc = 2,
        OcclusionSSSSSVelocityXVelocityYMisc = 3,
        Max = 4,
    };
}
namespace via::render {
    enum class CopyType : int64_t {
        copyResource = 0,
        copySubresourceRegion = 1,
    };
}
namespace app::ropeway::gui::FloorMapSt5DBehavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_5_211_0 = 1,
        Room_5_222_0 = 2,
    };
}
namespace via::render {
    enum class Comparison : int64_t {
        Never = 1,
        Less = 2,
        Equal = 3,
        LessEqual = 4,
        Greater = 5,
        NotEqual = 6,
        GreaterEqual = 7,
        Always = 8,
        Num = 9,
    };
}
namespace app::ropeway::gimmick::action::GimmickAutoDoor {
    enum class Routine : int64_t {
        CLOSE = 0,
        OPEN = 1,
    };
}
namespace via::hid::VrTracker {
    enum class PreferenceType : int64_t {
        FarPosition = 0,
        StablePosition = 1,
    };
}
namespace app::ropeway::CameraTwirler {
    enum class Operation : int64_t {
        CONSTANT = 0,
        ACCELARATION = 1,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class ConditionStateBitFlag2 : int64_t {
        STOP_DAMAGE = 0,
        STOP_WINCE = 1,
        STOP_BREAK = 2,
        STOP_REACTION = 3,
        STOP_ADDREACTION = 4,
        GUTS_MODE = 5,
        NO_DEATH = 6,
        AUTOPSIED = 7,
        ELIMINATED = 8,
        GRAPPLE_KILLING = 9,
        BACK_TERRITORY_NOT_CLEAR_HATE = 10,
    };
}
namespace via::motion::script::FootEffectController {
    enum class SETriggerTargetEnum : int64_t {
        PLAYER = 0,
        ENEMY = 1,
        NUM = 2,
    };
}
namespace app::ropeway::gui::RogueInventorySlotBehavior {
    enum class CommandType : int64_t {
        Invalid = 0xFFFFFFFF,
        Use = 0,
        Look = 1,
        ShortCut = 2,
        Equip = 3,
        RemoveEquip = 4,
        EquipSub = 5,
        RemoveEquipSub = 6,
        CustomOut = 7,
        Exchange = 8,
        Combine = 9,
        Remove = 10,
        ItemBoxIN_ = 11,
        ItemBoxOut = 12,
        GetItem = 13,
        Num = 14,
    };
}
namespace app::ropeway::gui::FloorMapSt5BBehavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        sm40_060_SecurityDoor_T03 = 1,
        sm40_058_PlantOfficeDoor_T02 = 2,
        sm40_146_AutoDoorCommon01A_T07 = 3,
        sm40_137_AutoDoor1m02A_T04 = 4,
        sm40_146_AutoDoorCommon01A_T06 = 5,
        sm40_062_IronDoor_T06 = 6,
    };
}
namespace app::ropeway::EquipmentDefine {
    enum class EquipStatus : int64_t {
        Invalid = 0,
        Equiped = 1,
        Holster = 2,
        Invisible = 3,
    };
}
namespace app::ropeway::enemy::em6300::fsmv2::action::Em6300FsmAction_SetMotionPattern {
    enum class PATTERN_TYPE : int64_t {
        WalkStart = 0,
        WalkTurn = 1,
        DashStart = 2,
        Stun = 3,
        KnockBack = 4,
        DashStagger = 5,
        AddDamage = 6,
        TargetLR = 7,
        SuperDashTurn = 8,
        GaripperStart = 9,
    };
}
namespace app::ropeway::survivor::fsmv2::action::SurvivorChangeWeaponAction {
    enum class TimingType : int64_t {
        OnStart = 0,
        OnEnd = 1,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class LoiteringActionPattern : int64_t {
        Action_00 = 0,
        Action_01 = 1,
        Action_02 = 2,
        Action_03 = 3,
        Action_04 = 4,
        Action_05 = 5,
        Action_06 = 6,
        Action_07 = 7,
        None = 99,
    };
}
namespace via::navigation {
    enum class PrimitiveHandleType : int64_t {
        None = 0,
        IndexedTriangle = 1,
        IndexedPolygon = 2,
        AABB2 = 3,
        LinkPrimArrow = 4,
    };
}
namespace via::areamap::test::DistanceFromNameRegion {
    enum class Condition : int64_t {
        Mode_Closest = 0,
        Mode_Farthest = 1,
    };
}
namespace app::ropeway::JackDominator {
    enum class ExecutePhase : int64_t {
        Immediate = 0,
        OnUpdate = 1,
        OnLateUpdate = 2,
    };
}
namespace app::ropeway::Em4100Define {
    enum class SetType : int64_t {
        SetType_Default = 0,
        SetType_City_A = 1,
        SetType_City_B = 2,
        SetType_City_C = 3,
        SetType_City_D = 4,
        SetType_City_E = 5,
        SetType_City_F = 6,
        SetType_City_G = 7,
    };
}
namespace via {
    enum class SystemServiceSkuFlag : int64_t {
        Unknown = 65535,
        Default = 0,
        Trial = 1,
        Full = 3,
    };
}
namespace via::memory {
    enum class CounterType : int64_t {
        UsedSize = 0,
        Overhead = 1,
        AllocCount = 2,
        FreeCount = 3,
        AllocSize = 4,
        FreeSize = 5,
    };
}
namespace via::navigation::AIMapEffector {
    enum class EffectType : int64_t {
        Attribute = 0,
        Disable = 1,
    };
}
namespace via::Rect3D {
    enum class VoronoiId : int64_t {
        L_Bit = 1,
        R_Bit = 2,
        T_Bit = 4,
        B_Bit = 8,
        Internal = 0,
        Edge_L_Bit = 1,
        Edge_R_Bit = 2,
        Edge_T_Bit = 4,
        Edge_B_Bit = 8,
        Vertex_LT_Bit = 5,
        Vertex_LB_Bit = 9,
        Vertex_RT_Bit = 6,
        Vertex_RB_Bit = 10,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class AttackKind : int64_t {
        HOLD = 0,
        BITE = 1,
        BACK_BITE = 2,
        LIGHTLY_HOLD = 3,
        PUSHDOWN = 4,
        DOWN_BITE = 5,
        CREEP_HOLD = 6,
        CREEP_BITE = 7,
        BITE_HOLD = 8,
        FREEHAND_BITE = 9,
        DIRECT_CREEP_BITE = 10,
        DUAL_BITE_A = 11,
        DUAL_BITE_B = 12,
        ONBOX_BITE = 13,
        TANGLE = 14,
    };
}
namespace via {
    enum class SystemServiceVideoRecorderStatus : int64_t {
        Unknown = 0,
        Idle = 1,
        Recording = 2,
    };
}
namespace via::areamap::RegionConnection {
    enum class AccessState : int64_t {
        Access_Open = 0,
        Access_Locked = 1,
        Access_Closed = 2,
    };
}
namespace app::ropeway::navigation::DirectLinkController {
    enum class LinkKind : int64_t {
        AtoB = 0,
        BtoA = 1,
        Both = 2,
        Invalid = 3,
    };
}
namespace app::ropeway::gui::MapBehavior {
    enum class OpenMode : int64_t {
        INVALID = 0xFFFFFFFF,
        DIRECT = 0,
        DIRECT_NO_PAUSE = 1,
        FROM_MENU = 2,
        MINI_MAP = 3,
        DEMO_4TH = 4,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class Knock : int64_t {
        WALL_00 = 0,
        FENCE_00 = 1,
        FENCE_01 = 2,
        FENCE_02 = 3,
        FENCE_03 = 4,
        PRISON_00 = 5,
        PRISON_01 = 6,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class Em0000DirtyPreset_Body : int64_t {
        Body00 = 0,
        Body01 = 1,
    };
}
namespace via::render {
    enum class UavDimension : int64_t {
        Unknown = 0,
        Buffer = 1,
        Texture1d = 2,
        Texture1darray = 3,
        Texture2d = 4,
        Texture2darray = 5,
        Texture3d = 8,
    };
}
namespace app::ropeway::weapon::shell::ShellManager {
    enum class State : int64_t {
        INVALID = 0,
        PLAYER_GENERATED = 1,
    };
}
namespace app::ropeway::enemy::em3000::fsmv2::action::Em3000FsmAction_SetTargetAngle {
    enum class EXEC_TYPE : int64_t {
        START = 0,
        END = 1,
        FRAME = 2,
    };
}
namespace app::ropeway::weapon::generator::BombGeneratorUserDataBase{
    enum class BombEmberGeneratorSetting : int64_t {
        S = 0,
        M = 1,
        L = 2,
    };
}
namespace app::ropeway::GimmickOptionDoorSettings {
    enum class UnlockSettingType : int64_t {
        NONE = 0,
        BOTH = 1,
        SIDE_AB = 2,
    };
}
namespace via {
    enum class VideoRecordingStatus : int64_t {
        Disable = 0xFFFFFFFF,
        NotSupported = -2,
        None = 0,
        Running = 1,
        Paused = 2,
        Ready = 3,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0100_MONTAGE_PARTS_BODY : int64_t {
        BODY00 = 0,
        NONE = 1,
    };
}
namespace app::ropeway::gui::KeyBindBehavior {
    enum class ItemMode : int64_t {
        SELECTOR = 0,
        SELECTOR_HDRMODE = 1,
        SELECTOR_WINDOWMODE = 2,
        SELECTOR_RENET = 3,
        SLIDER = 4,
        EXECUTER = 5,
        EXECUTER2 = 6,
    };
}
namespace via::wwise::WwiseGeneratedBankInfo {
    enum class IdType : int64_t {
        None = 0,
        Bank = 1,
        Event = 2,
        SwitchGroup = 3,
        Switch = 4,
        Rtpc = 5,
        Trigger = 6,
        StateGroup = 7,
        State = 8,
        AudioBus = 9,
        AuxiliaryBus = 10,
        InMemoryAudio = 11,
        StreamedAudio = 12,
    };
}
namespace via::os {
    enum class MemoryProtection : int64_t {
        CPU_RO = 4,
        CPU_RW = 8,
        GPU_RO = 16,
        GPU_WO = 32,
        GPU_RW = 48,
        Default = 56,
    };
}
namespace via::motion::JointExLimit {
    enum class LimitFlags : int64_t {
        None = 0,
        TransMinX = 1,
        TransMaxX = 2,
        TransMinY = 4,
        TransMaxY = 8,
        TransMinZ = 16,
        TransMaxZ = 32,
        RotMinX = 64,
        RotMinY = 128,
        RotMinZ = 256,
        RotMaxX = 512,
        RotMaxY = 1024,
        RotMaxZ = 2048,
        ScaleMinX = 4096,
        ScaleMinY = 8192,
        ScaleMinZ = 16384,
        ScaleMaxX = 32768,
        ScaleMaxY = 65536,
        ScaleMaxZ = 131072,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class ScenePhase : int64_t {
        FASTEST = -86,
        GuiManager = -85,
        Gui = -84,
        CutSceneManager = -83,
        CutScene = -82,
        CutSceneActor = -81,
        LATEST = -80,
    };
}
namespace app::ropeway::weapon::shell::ShellDefine {
    enum class FolderTypeKind : int64_t {
        Weapon = 0,
        Enemy = 1,
    };
}
namespace via::math {
    enum class RotationOrder : int64_t {
        XYZ = 0,
        YZX = 1,
        ZXY = 2,
        ZYX = 3,
        YXZ = 4,
        XZY = 5,
    };
}
namespace app::ropeway::gui::NewInventoryDetailBehavior {
    enum class PuzzleModeEnum : int64_t {
        None = 0,
        PortableSafe = 1,
        CarKey = 2,
        Oscilloscope = 3,
    };
}
namespace via::effect {
    enum class PlayerState : int64_t {
        Idle = 0,
        Start = 1,
        Running = 2,
        ForceStop = 3,
        Finish = 4,
        Finished = 5,
        Restart = 6,
    };
}
namespace via::motion::detail::ChainLinkData {
    enum class AttrFlags : int64_t {
        EnableStretch = 1,
    };
}
namespace via::DateTime {
    enum class Kind : int64_t {
        Unspecified = 0,
        UTC = 1,
        Local = 2,
    };
}
namespace app::ropeway::gui::GimmickSprayingMachineGuiBehavior {
    enum class StateType : int64_t {
        NONE = 0,
        SCREENSAVER = 1,
        DEFAULT = 2,
        SUCCESS = 3,
        ERROR_EMPTY_ = 4,
        ERROR_TEMPERATURE = 5,
    };
}
namespace via::network::AutoMatchmaking {
    enum class RuleType : int64_t {
        Must = 0,
        Should = 1,
    };
}
namespace via {
    enum class Hide : int64_t {
        Script = 1,
        Tool = 2,
        Public = 4,
    };
}
namespace via::render::layer::PrepareOutput {
    enum class DISPLAYCOLORSPACE : int64_t {
        SRGB = 0,
        HDTV_REC709 = 1,
        BT2020 = 2,
    };
}
namespace System::Threading {
    enum class ThreadState : int64_t {
        Running = 0,
        StopRequested = 1,
        SuspendRequested = 2,
        Background = 4,
        Unstarted = 8,
        Stopped = 16,
        WaitSleepJoIN_ = 32,
        Suspended = 64,
        AbortRequested = 128,
        Aborted = 256,
    };
}
namespace app::ropeway::fsmv2::GmkCustomJackHelperOn {
    enum class StateType : int64_t {
        BEFORE = 0,
        PLAY = 1,
        AFTER = 2,
    };
}
namespace via::motion {
    enum class SequenceUpdateMode : int64_t {
        Always = 0,
        PlayOnly = 1,
        DisableReverseEvent = 2,
        Disable = 3,
    };
}
namespace via::render::layer::CaptureGBuffer {
    enum class FACEINDEX : int64_t {
        POSITIVE_X = 0,
        NEGATIVE_X = 1,
        POSITIVE_Y = 2,
        NEGATIVE_Y = 3,
        POSITIVE_Z = 4,
        NEGATIVE_Z = 5,
    };
}
namespace via::navigation::FailReport {
    enum class FailLevel : int64_t {
        Upper = 0,
        Lower = 1,
    };
}
namespace app::ropeway::behaviortree::BehaviorTreeDefine {
    enum class CompareType : int64_t {
        Equal = 0,
        NotEqual = 1,
        Less = 2,
        LessEq = 3,
        Greater = 4,
        GreaterEq = 5,
    };
}
namespace via::areamap::test::BasicDistance {
    enum class Condition : int64_t {
        Mode_Closest = 0,
        Mode_Farthest = 1,
    };
}
namespace app::ropeway::OptionManager {
    enum class AntiAliasQuality : int64_t {
        OFF = 0,
        FXAA = 1,
        TAA = 2,
        FXAA_TAA = 3,
        SMAA = 4,
    };
}
namespace app::ropeway::gui::SelectBrightnessBehaviorMin {
    enum class EndType : int64_t {
        SELECTED = 0,
        CANCELED = 1,
    };
}
namespace app::ropeway::gui::FloatIconParam {
    enum class FloatIconCollisionType : int64_t {
        DEFAULT = 0xFFFFFFFF,
        TYPE_A = 0,
        TYPE_B = 1,
        TYPE_C = 2,
        TYPE_D = 3,
        TYPE_E = 4,
        TYPE_F = 5,
    };
}
namespace via::motion::SyncPointData {
    enum class SyncType : int64_t {
        Loop = 0,
        Once = 1,
        Both = 2,
    };
}
namespace app::ropeway::enemy::em6000::MotionPattern {
    enum class DirFBLR : int64_t {
        Front = 0,
        Back = 1,
        Left = 2,
        Right = 3,
    };
}
namespace via::gui {
    enum class LightGroupBit : int64_t {
        Group0 = 1,
        Group1 = 2,
        Group2 = 4,
        Group3 = 8,
        Group4 = 16,
        Group5 = 32,
        Group6 = 64,
        Group7 = 128,
        All = 255,
    };
}
namespace via::navigation::map::NodeBlock {
    enum class Attribute : int64_t {
        Static = 0,
        ModifyBase = 1,
    };
}
namespace app::ropeway::SoundColliderUserData {
    enum class BodyParts : int64_t {
        Head = 0,
        Body = 1,
        UpperArm = 2,
        Forearm = 3,
        Thigh = 4,
        ShIN_ = 5,
    };
}
namespace via::motion::SubExJointRemapInput {
    enum class JointLimitFlags : int64_t {
        BasePose = 1,
        MidPoint = 2,
        Enable = 4,
    };
}
namespace via::motion::ChainResource {
    enum class SettingAttrFlags : int64_t {
        None = 0,
        Default = 1,
        VirtualGroundRoot = 2,
        VirtualGroundTarget = 4,
        IgnoreSameGroupCollision = 8,
        VirtualGroundMask = 6,
    };
}
namespace via::gui::detail {
    enum class SubFontNo : int64_t {
        No0 = 0,
        No1 = 1,
        Max = 2,
    };
}
namespace app::ropeway::posteffect::SetPostEffectParam {
    enum class SettingMode : int64_t {
        Param = 0,
        Preset = 1,
        Default = 2,
        Relay = 3,
        RelayNoRecursive = 4,
        OutOfSurvice = 5,
    };
}
namespace app::ropeway::environment::GameObjectSwitcher::Order {
    enum class TimingType : int64_t {
        OnContact = 0,
        OnSeparate = 1,
    };
}
namespace via::dialog {
    enum class Status : int64_t {
        None = 0,
        Initialized = 1,
        Running = 2,
        Finished = 3,
    };
}
namespace via::effect::script::RecordSystem::RecordOrder {
    enum class OrderTypeEnum : int64_t {
        Stamp = 1,
        Decal = 2,
        StampAndDecal = 3,
    };
}
namespace via::effect {
    enum class VectorFieldDrawFlag : int64_t {
        None = 0,
        Box = 1,
        Grid = 2,
    };
}
namespace via::render::RenderConfig {
    enum class LensDistortionSetting : int64_t {
        ON = 0,
        DistortionOnly = 1,
        OFF = 2,
    };
}
namespace app::ropeway::enemy::em6200 {
    enum class ColliderSkipID : int64_t {
        ThroughThrow = 1,
        PressDefault = 2,
        PressSleep = 4,
    };
}
namespace app::ropeway::enemy::EnemyMotseqController {
    enum class RotateDirectionMode : int64_t {
        Both = 0,
        LeftOnly = 1,
        RightOnly = 2,
    };
}
namespace app::ropeway::fsmv2 {
    enum class ValueSetType : int64_t {
        KEEP = 0,
        RESET = 1,
        SET = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class BreakPartsIgnoreOption : int64_t {
        EFFECT_OPT_00 = 0,
        EFFECT_OPT_01 = 1,
        EFFECT_OPT_02 = 2,
        EFFECT_OPT_03 = 3,
        SOUND_OPT_00 = 4,
        SOUND_OPT_01 = 5,
        SOUND_OPT_02 = 6,
        SOUND_OPT_03 = 7,
    };
}
namespace app::ropeway::enemy::em6300::fsmv2::action::MotionCameraPlayRequest {
    enum class CameraPatternType : int64_t {
        Rush_Kill = 0,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::EmCommonFsmAction_MovePosition {
    enum class AXIS : int64_t {
        X = 0,
        Y = 1,
        Z = 2,
        InvX = 3,
        InvY = 4,
        InvZ = 5,
        WorldY = 6,
        WorldInvY = 7,
    };
}
namespace app::ropeway::gimmick::action::GimmickMobileBookShelf {
    enum class ActionType : int64_t {
        HOLD = 0,
        RELEASE = 1,
        ENTER = 2,
        NONE = 3,
    };
}
namespace app::ropeway::EquipmentDefine {
    enum class DisplayControlTarget : int64_t {
        None = 0,
        MaIN_ = 1,
        Sub = 2,
        Holsters = 4,
        FlashLight = 8,
        FlashLightSource = 16,
        Reserve05 = 32,
        Reserve06 = 64,
        Other = 128,
        All = 255,
    };
}
namespace app::ropeway::enemy::em6000::MotionPattern {
    enum class DirLR : int64_t {
        Left = 0,
        Right = 1,
    };
}
namespace app::ropeway::gui::FloorMapSt4BBehavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_4_201_0e = 1,
        Room_4_201_0f = 2,
        Room_4_701_0a = 3,
        Room_4_701_0b = 4,
        Room_4_701_0c = 5,
        Room_4_702_0 = 6,
        Room_4_703_0 = 7,
        Room_4_704_0a = 8,
        Room_4_704_0b = 9,
        Room_4_708_0 = 10,
        Room_4_710_0 = 11,
        Room_4_713_0 = 12,
        Room_4_714_0a = 13,
        Room_4_714_0b = 14,
        Room_4_714_0c = 15,
        Room_4_716_0 = 16,
        Room_4_754_0 = 17,
    };
}
namespace via::motion::detail {
    enum class ChainCollisionShape : int64_t {
        None = 0,
        Sphere = 1,
        Capsule = 2,
        LineSphere = 3,
        LerpSphere = 4,
    };
}
namespace app::ropeway::gamemastering::RecordManager {
    enum class ProgressType : int64_t {
        NONE = 0,
        COUNT_UP = 1,
        COUNT_DOWN = 2,
        CLEARTIME = 3,
        ITEMBOX = 4,
        CURE = 5,
        WALK = 6,
        ADA_GUN = 7,
    };
}
namespace app::ropeway::weapon::shell::ShellCartridgeManager {
    enum class State : int64_t {
        Invalid = 0,
        IN_GAME = 1,
    };
}
namespace via::motion::IkLeg {
    enum class EffectorOffsetCtrl : int64_t {
        None = 0,
        Local = 1,
        World = 2,
    };
}
namespace app::ropeway::network::service::StoreServiceController {
    enum class ProcessType : int64_t {
        Idle = 0,
        Requested = 1,
        ContextWait = 2,
        Setup = 3,
        Open = 4,
        OpenWait = 5,
        Close = 6,
    };
}
namespace via::os {
    enum class ChunkInstalledDevice : int64_t {
        None = 0,
        Slow = 1,
        Fast = 2,
    };
}
namespace app::ropeway::enemy::em6000::fsmv2::action::MotionCameraPlayRequest {
    enum class CameraPatternType : int64_t {
        Hold_Start = 0,
        Hold_Loop = 1,
        Hold_Escape = 2,
        Hold_Death = 3,
        Hold_Use_Knife = 4,
        Hold_Use_Grenade = 5,
    };
}
namespace via::userdata::MultiLogicNode {
    enum class Operation : int64_t {
        And = 0,
        Or = 1,
    };
}
namespace via::motion::MotionAppendData {
    enum class PrimitiveClassType : int64_t {
        Unkonw = 0,
        Vec2 = 1,
        Vec3 = 2,
        Vec4 = 3,
        Quaternion = 4,
        Matrix = 5,
    };
}
namespace via::render {
    enum class ShadowPrecision : int64_t {
        Default = 0,
        Low = 1,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class MapPhase : int64_t {
        FASTEST = -80,
        ScenarioController = -79,
        GimmickInteractManager = -78,
        GimmickManager = -77,
        GimmickItemPositions = -76,
        GimmickTrigger = -75,
        GimmickActionPre = -74,
        GimmickActionControl = -73,
        GimmickActionBodyBeforeEnemy = -72,
        GimmickActionBody = -71,
        UiMapGUI = -70,
        FsmStateManager = -69,
        FsmStateControl = -68,
        AfterFsmStateControl = -67,
        LATEST = -66,
    };
}
namespace app::ropeway::enemy::em9000::Em9000Think {
    enum class HandType : int64_t {
        Default = 0,
        Left = 1,
        Right = 2,
    };
}
namespace via::render {
    enum class AmbientBRDF : int64_t {
        Legacy = 0,
        New = 1,
    };
}
namespace via::hid::VrTracker {
    enum class MotionSensorDataUpdateTiming : int64_t {
        Nothing = 0,
        Always = 2147483647,
        OnLedTrackerProcessSuccessful = 4,
        OnLedTrackerProcessFailed = 3,
        OnVrTrackerGpuSubmitFailed = 1,
        OnVrTrackerGpuWaitAndCpuProcessFailed = 2,
    };
}
namespace app::ropeway::gamemastering::StaffRollFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        WAIT_GUI_CREATION = 1,
        UPDATE = 2,
        FINALIZE = 3,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace via::hid::camera {
    enum class PlayStationCameraCaptureFrameResult : int64_t {
        OK = 1,
        ErrorStoped = 2147483649,
        ErrorDeviceDisconnected = 2147483650,
        ErrorInternal = 2147487744,
    };
}
namespace app::ropeway::enemy::em6300::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_FIRST_THREAT = 2,
        ACT_SECOND_THREAT = 3,
        ACT_THREAT = 4,
        ACT_YOROKE = 5,
        ATK_NAIL = 2000,
        ATK_NAIL2 = 2001,
        ATK_NAIL_BACK = 2002,
        ATK_THRUST = 2003,
        ATK_RUBBLE_UPPER = 2004,
        ATK_GARIPPER = 2005,
        ATK_POWER_DUNK = 2006,
        ATK_GRAPPLE_KILL = 2007,
        DMG_STUN = 3000,
        DMG_KNOCKBACK = 3001,
        DMG_FLASH = 3002,
        DMG_BOMB = 3003,
        DIE_NORMAL = 5000,
        DIE_BOMB = 5001,
        DIE_RIGID = 5002,
        DIE_NORMAL_ROGUE = 5003,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace app::ropeway::gamemastering::LModeSetupFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        GUI_CREATION_TO_INGAME = 1,
        WAIT_GUI_CREATION_TO_INGAME = 2,
        UPDATE = 3,
        WAIT_FADE = 4,
        TO_INGAME = 5,
        FINALIZE = 6,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace app::ropeway::gimmick::action::GimmickMedicineMachine {
    enum class RotateType : int64_t {
        A_B = 0,
        B_C = 1,
    };
}
namespace app::ropeway::enemy::em9000::BehaviorTreeAction {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_RUN = 2,
        ACT_TURN = 3,
        ATK_ATTACK = 2000,
        ATK_GRAPPLE_DEAD = 2001,
    };
}
namespace app::ropeway::survivor::npc::NpcOrderDefine {
    enum class OrderGroup : int64_t {
        Common = 0,
        Manage = 1,
        Unique = 2,
        Move = 3,
        Action = 4,
        Think = 5,
        LookAt = 6,
        Invalid = 7,
    };
}
namespace via::motion::script::CharacterSetting {
    enum class MoveTypeEnum : int64_t {
        Bipedal = 0,
        Quadrupedal = 1,
        Both = 2,
        Other = 3,
    };
}
namespace app::ropeway::StandbyMediator {
    enum class Step : int64_t {
        IDLE = 0,
        KICK_STANDBY = 1,
        WAIT_LOAD = 2,
        BurstMode = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickBody {
    enum class GimmickClass : int64_t {
        UNSET = 0,
        DOOR = 1,
        WINDOW = 2,
        LADDER = 3,
        AUTODOOR = 4,
    };
}
namespace app::ropeway::RestrictManager {
    enum class RestrictMode : int64_t {
        INVALID = 0,
        IN_GAME = 1,
        SYSTEM_00 = 2,
        SYSTEM_01 = 3,
        SYSTEM_02 = 4,
        ENEMY = 5,
        EVENT = 6,
        EVENT_PAUSE = 7,
        GIMMICK_00 = 8,
        GIMMICK_01 = 9,
        GIMMICK_02 = 10,
        GIMMICK_03 = 11,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class MoveEndAngle : int64_t {
        NONE = 0,
        ANY = 1,
        PLAYER = 2,
    };
}
namespace via::gui {
    enum class RegionFitType : int64_t {
        None = 0,
        Horizontal = 1,
        Vertical = 2,
        Both = 3,
    };
}
namespace via::motion::SubExJointRemapInput {
    enum class Axis : int64_t {
        X = 0,
        Y = 1,
        Z = 2,
    };
}
namespace app::ropeway::gui::LoadingIconBehavior {
    enum class OpenType : int64_t {
        Default = 0,
        Costume = 1,
        FigureList = 2,
        FigureDetail = 3,
        Map = 4,
        FileList = 5,
        FileDetail = 6,
        ConceptArtDetail = 7,
        OptionGraphic = 8,
    };
}
namespace System::Threading {
    enum class ApartmentState : int64_t {
        STA = 0,
        MTA = 1,
        Unknown = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickWindow {
    enum class ExitType : int64_t {
        LARGE = 0,
        CRAMPED = 1,
    };
}
namespace app::ropeway::gui::FloorMapSt42Behavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        Door_2_1_101 = 1,
        Door_2_1_102 = 2,
        Door_2_1_103 = 3,
        Door_2_1_107 = 4,
        sm40_015_STARSArmoryDoor01A = 5,
        Door_2_1_110 = 6,
        Door_2_1_111 = 7,
        Door_2_1_112 = 8,
        Door_2_1_114 = 9,
        Door_2_1_116 = 10,
        Door_2_1_117 = 11,
        Door_2_1_118 = 12,
        Door_2_1_119 = 13,
        Door_2_1_120 = 14,
        Door_2_1_121 = 15,
        Door_2_1_122 = 16,
        Door_2_1_126 = 17,
    };
}
namespace app::ropeway::gimmick::action::GimmickDoorBase {
    enum class SideType : int64_t {
        SIDE_A = 0,
        SIDE_B = 1,
        NONE = 2,
    };
}
namespace app::ropeway::EnvironmentStandbyController {
    enum class Step : int64_t {
        None = 0,
        NoneToReservation = 1,
        ReservationToNone = 2,
        Reservation = 3,
        ReservationToStandbyScene = 4,
        ReservationToStandbyAdditionalScene = 5,
        StandbyToReservationScene = 6,
        StandbyToReservationAdditionalScene = 7,
        Standby = 8,
        StandbyToActiveAdditionalScene = 9,
        ActiveToStandbyAdditionalScene = 10,
        Active = 11,
    };
}
namespace app::ropeway {
    enum class IkUpdatePhase : int64_t {
        MainUpdate = 0,
        FirstUpdate = 1,
        SecondUpdate = 2,
    };
}
namespace app::ropeway::effect::script::EmitZoneERMRegister {
    enum class EmitZoneType : int64_t {
        RaIN_ = 0,
        WarterArea = 1,
        None = 2,
    };
}
namespace via::motion {
    enum class SequencePhase : int64_t {
        Init = 0,
        First = 1,
        Second = 2,
        Normal = 3,
    };
}
namespace via::Component {
    enum class TypeFlag : int64_t {
        CollectionIndexEnable = 1,
        BehaviorGroupIndexEnable = 2,
        CollectionIndexMask = 16711680,
        BehaviorGroupIndexMask = 4278190080,
    };
}
namespace via::render::LDRRadialBlur {
    enum class LookAtType : int64_t {
        WorldPosition = 0,
        ScreenPosition = 1,
    };
}
namespace via::render::MeshRenderer {
    enum class Status : int64_t {
        None = 0,
        BeginRendering = 1,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0100_MONTAGE_PARTS_HAT : int64_t {
        NONE = 0,
    };
}
namespace app::ropeway::enemy::em5000::savedata::Em5000SaveData {
    enum class StatusFlagBitIndex : int64_t {
        FALL_END = 0,
        FAKE_DEAD = 1,
        BODY_DAMAGED = 2,
    };
}
namespace via::effect::detail {
    enum class LuminanceBleedType : int64_t {
        None = 0,
        Transparent = 1,
        PostTransparent = 2,
        ForceWord = 3,
    };
}
namespace app::ropeway::EquipmentDefine {
    enum class WeaponType : int64_t {
        Invalid = 0xFFFFFFFF,
        BareHand = 0,
        WP0000 = 1,
        WP0100 = 2,
        WP0200 = 3,
        WP0300 = 4,
        WP0400 = 5,
        WP0500 = 6,
        WP0600 = 7,
        WP0700 = 8,
        WP0800 = 9,
        WP0900 = 10,
        WP1000 = 11,
        WP1100 = 12,
        WP1200 = 13,
        WP1300 = 14,
        WP1400 = 15,
        WP1500 = 16,
        WP1600 = 17,
        WP1700 = 18,
        WP1800 = 19,
        WP1900 = 20,
        WP2000 = 21,
        WP2100 = 22,
        WP2200 = 23,
        WP2300 = 24,
        WP2400 = 25,
        WP2500 = 26,
        WP2600 = 27,
        WP2700 = 28,
        WP2800 = 29,
        WP2900 = 30,
        WP3000 = 31,
        WP3100 = 32,
        WP3200 = 33,
        WP3300 = 34,
        WP3400 = 35,
        WP3500 = 36,
        WP3600 = 37,
        WP3700 = 38,
        WP3800 = 39,
        WP3900 = 40,
        WP4000 = 41,
        WP4100 = 42,
        WP4200 = 43,
        WP4300 = 44,
        WP4400 = 45,
        WP4500 = 46,
        WP4510 = 47,
        WP4520 = 48,
        WP4600 = 49,
        WP4700 = 50,
        WP4800 = 51,
        WP4900 = 52,
        WP5000 = 53,
        WP5100 = 54,
        WP5200 = 55,
        WP5300 = 56,
        WP5400 = 57,
        WP5500 = 58,
        WP5600 = 59,
        WP5700 = 60,
        WP5800 = 61,
        WP5900 = 62,
        WP6000 = 63,
        WP6100 = 64,
        WP6200 = 65,
        WP6300 = 66,
        WP6400 = 67,
        WP6410 = 68,
        WP6420 = 69,
        WP6430 = 70,
        WP6440 = 71,
        WP6450 = 72,
        WP6460 = 73,
        WP6470 = 74,
        WP6480 = 75,
        WP6490 = 76,
        WP6500 = 77,
        WP6600 = 78,
        WP6700 = 79,
        WP6800 = 80,
        WP6900 = 81,
        WP7000 = 82,
        WP7010 = 83,
        WP7020 = 84,
        WP7030 = 85,
        WP7040 = 86,
        WP7050 = 87,
        WP7060 = 88,
        WP7070 = 89,
        WP7080 = 90,
        WP7090 = 91,
        WP7100 = 92,
        WP7110 = 93,
        WP7120 = 94,
        WP7130 = 95,
        WP7140 = 96,
        WP7150 = 97,
        WP7160 = 98,
        WP7170 = 99,
        WP7180 = 100,
        WP7190 = 101,
        WP7200 = 102,
        WP7210 = 103,
        WP7220 = 104,
        WP7230 = 105,
        WP7240 = 106,
        WP7250 = 107,
        WP7260 = 108,
        WP7270 = 109,
        WP7280 = 110,
        WP7290 = 111,
        WP7300 = 112,
        WP7310 = 113,
        WP7320 = 114,
        WP7330 = 115,
        WP7340 = 116,
        WP7350 = 117,
        WP7360 = 118,
        WP7370 = 119,
        WP7380 = 120,
        WP7390 = 121,
        WP7400 = 122,
        WP7410 = 123,
        WP7420 = 124,
        WP7430 = 125,
        WP7440 = 126,
        WP7450 = 127,
        WP7460 = 128,
        WP7470 = 129,
        WP7480 = 130,
        WP7490 = 131,
        WP7500 = 132,
        WP7510 = 133,
        WP7520 = 134,
        WP7530 = 135,
        WP7540 = 136,
        WP7550 = 137,
        WP7560 = 138,
        WP7570 = 139,
        WP7580 = 140,
        WP7590 = 141,
        WP7600 = 142,
        WP7610 = 143,
        WP7620 = 144,
        WP7630 = 145,
        WP7640 = 146,
        WP7650 = 147,
        WP7660 = 148,
        WP7670 = 149,
        WP7680 = 150,
        WP7690 = 151,
        WP7700 = 152,
        WP7710 = 153,
        WP7720 = 154,
        WP7730 = 155,
        WP7740 = 156,
        WP7750 = 157,
        WP7760 = 158,
        WP7770 = 159,
        WP7780 = 160,
        WP7790 = 161,
        WP7800 = 162,
        WP7810 = 163,
        WP7820 = 164,
        WP7830 = 165,
        WP7840 = 166,
        WP7850 = 167,
        WP7860 = 168,
        WP7870 = 169,
        WP7880 = 170,
        WP7890 = 171,
        WP7900 = 172,
        WP7910 = 173,
        WP7920 = 174,
        WP7930 = 175,
        WP7940 = 176,
        WP7950 = 177,
        WP7960 = 178,
        WP7970 = 179,
        WP7980 = 180,
        WP7990 = 181,
        WP8000 = 182,
        WP8010 = 183,
        WP8020 = 184,
        WP8030 = 185,
        WP8040 = 186,
        WP8050 = 187,
        WP8060 = 188,
        WP8070 = 189,
        WP8080 = 190,
        WP8090 = 191,
        WP8100 = 192,
        WP8110 = 193,
        WP8120 = 194,
        WP8130 = 195,
        WP8140 = 196,
        WP8150 = 197,
        WP8160 = 198,
        WP8170 = 199,
        WP8180 = 200,
        WP8190 = 201,
        WP8200 = 202,
        WP8210 = 203,
        WP8220 = 204,
        WP8230 = 205,
        WP8240 = 206,
        WP8250 = 207,
        WP8260 = 208,
        WP8270 = 209,
        WP8280 = 210,
        WP8290 = 211,
        WP8300 = 212,
        WP8310 = 213,
        WP8320 = 214,
        WP8330 = 215,
        WP8340 = 216,
        WP8350 = 217,
        WP8360 = 218,
        WP8370 = 219,
        WP8380 = 220,
        WP8390 = 221,
        WP8400 = 222,
        WP8410 = 223,
        WP8420 = 224,
        WP8430 = 225,
        WP8440 = 226,
        WP8450 = 227,
        WP8460 = 228,
        WP8470 = 229,
        WP8480 = 230,
        WP8490 = 231,
        WP8500 = 232,
        WP8510 = 233,
        WP8520 = 234,
        WP8530 = 235,
        WP8540 = 236,
        WP8550 = 237,
        WP8560 = 238,
        WP8570 = 239,
        WP8580 = 240,
        WP8590 = 241,
        WP8600 = 242,
        WP8610 = 243,
        WP8620 = 244,
        WP8630 = 245,
        WP8640 = 246,
        WP8650 = 247,
        WP8660 = 248,
        WP8670 = 249,
        WP8680 = 250,
        WP8690 = 251,
        WP8700 = 252,
        WP8710 = 253,
        WP8720 = 254,
        WP8730 = 255,
        WP8740 = 256,
        WP8750 = 257,
        WP8760 = 258,
        WP8770 = 259,
        WP8780 = 260,
        WP8790 = 261,
        WP8800 = 262,
        WP8810 = 263,
        WP8820 = 264,
        WP8830 = 265,
        WP8840 = 266,
        WP8850 = 267,
        WP8860 = 268,
        WP8870 = 269,
        WP8880 = 270,
        WP8890 = 271,
        WP8900 = 272,
        WP8910 = 273,
        WP8920 = 274,
        WP8930 = 275,
        WP8940 = 276,
        WP8950 = 277,
        WP8960 = 278,
        WP8970 = 279,
        WP8980 = 280,
        WP8990 = 281,
        WP9000 = 282,
        WP9010 = 283,
        WP9020 = 284,
        WP9030 = 285,
        WP9040 = 286,
        WP9050 = 287,
        WP9060 = 288,
        WP9070 = 289,
        WP9080 = 290,
        WP9090 = 291,
        WP9100 = 292,
        WP9110 = 293,
        WP9120 = 294,
        WP9130 = 295,
        WP9140 = 296,
        WP9150 = 297,
        WP9160 = 298,
        WP9170 = 299,
        WP9180 = 300,
        WP9190 = 301,
        WP9200 = 302,
        WP9210 = 303,
        WP9220 = 304,
        WP9230 = 305,
        WP9240 = 306,
        WP9250 = 307,
        WP9260 = 308,
        WP9270 = 309,
        WP9280 = 310,
        WP9290 = 311,
        WP9300 = 312,
        WP9310 = 313,
        WP9320 = 314,
        WP9330 = 315,
        WP9340 = 316,
        WP9350 = 317,
        WP9360 = 318,
        WP9370 = 319,
        WP9380 = 320,
        WP9390 = 321,
        WP9400 = 322,
        WP9410 = 323,
        WP9420 = 324,
        WP9430 = 325,
        WP9440 = 326,
        WP9450 = 327,
        WP9460 = 328,
        WP9470 = 329,
        WP9480 = 330,
        WP9490 = 331,
        WP9500 = 332,
        WP9510 = 333,
        WP9520 = 334,
        WP9530 = 335,
        WP9540 = 336,
        WP9550 = 337,
        WP9560 = 338,
        WP9570 = 339,
        WP9580 = 340,
        WP9590 = 341,
        WP9600 = 342,
        WP9610 = 343,
        WP9620 = 344,
        WP9630 = 345,
        WP9640 = 346,
        WP9650 = 347,
        WP9660 = 348,
        WP9670 = 349,
        WP9680 = 350,
        WP9690 = 351,
        WP9700 = 352,
        WP9710 = 353,
        WP9720 = 354,
        WP9730 = 355,
        WP9740 = 356,
        WP9750 = 357,
        WP9760 = 358,
        WP9770 = 359,
        WP9780 = 360,
        WP9790 = 361,
        WP9800 = 362,
        WP9810 = 363,
        WP9820 = 364,
        WP9830 = 365,
        WP9840 = 366,
        WP9850 = 367,
        WP9860 = 368,
        WP9870 = 369,
        WP9880 = 370,
        WP9890 = 371,
        WP9900 = 372,
        WP9910 = 373,
        WP9920 = 374,
        WP9930 = 375,
        WP9940 = 376,
        WP9950 = 377,
        WP9960 = 378,
        WP9970 = 379,
        WP9980 = 380,
        WP9990 = 381,
    };
}
namespace app::ropeway::gamemastering::AchievementDefine {
    enum class RogueID : int64_t {
        ROGUE_SILVER_000 = 0,
        ROGUE_BRONZE_000 = 1,
    };
}
namespace via::effect {
    enum class WindInfluenceDrawFlag : int64_t {
        None = 0,
        Select = 1,
        All = 2,
    };
}
namespace app::ropeway::Em6200AttackSensor {
    enum class Sensor : int64_t {
        Upper = 0,
        Lower = 1,
    };
}
namespace via::render {
    enum class HazeCompositorFilterType : int64_t {
        Variance = 0,
        AdjacentClamping = 1,
        Gaussian = 2,
    };
}
namespace via::network::utility::Request {
    enum class State : int64_t {
        Dead = 0,
        Init = 1,
        Start = 2,
        StartFail = 3,
        Update = 4,
        Finish = 5,
    };
}
namespace app::ropeway::enemy::em6200::MotionPattern {
    enum class DirFBLR : int64_t {
        Front = 0,
        Back = 1,
        Left = 2,
        Right = 3,
    };
}
namespace via::fsm::FsmManager {
    enum class SelectorTiming : int64_t {
        Last = 0,
        BackGround = 1,
    };
}
namespace via::nnfc {
    enum class DeviceState : int64_t {
        Initialize = 0,
        NoDevice = 1,
        Conflict = 2,
        Ready = 3,
        Detect = 4,
        Mount = 5,
        Panic = 6,
    };
}
namespace via::browser {
    enum class RequestId : int64_t {
        ContextStart = 257,
        BrowserOpen = 513,
        BrowserClose = 514,
    };
}
namespace via::dynamics::gjk::SimplexSolver {
    enum class SimplexType : int64_t {
        None = 0,
        Point = 1,
        LineSegment = 2,
        Triangle = 3,
        Tetrahedron = 4,
        Max = 5,
    };
}
namespace app::ropeway::fsmv2::LookActionTarget {
    enum class LookMode : int64_t {
        OWNER_LOOK_TARGET = 0,
        TARGET_LOOK_OWNER = 1,
    };
}
namespace app::ropeway::survivor::ArrivalController {
    enum class UserKind : int64_t {
        Own = 0,
        CompareTarget = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickDoorBase {
    enum class AddEndType : int64_t {
        KEEP_OPENED = 0,
        CLOSED = 1,
        WAIT_CLOSE = 2,
        CLOSE_THEN_UNLOCK = 3,
    };
}
namespace app::ropeway::gimmick::option::EnemyControlSettings::EnemyControlParam {
    enum class SetThinkType : int64_t {
        Standalone = 0,
        FsmControl = 1,
    };
}
namespace app::ropeway::gamemastering::RogueRecordManager {
    enum class RogueRecordId : int64_t {
        ROGUE_RECORD_00 = 0,
        ROGUE_RECORD_01 = 1,
        ROGUE_RECORD_02 = 2,
        ROGUE_RECORD_03 = 3,
        ROGUE_RECORD_04 = 4,
        ROGUE_RECORD_05 = 5,
        ROGUE_RECORD_06 = 6,
        ROGUE_RECORD_07 = 7,
        ROGUE_RECORD_08 = 8,
        ROGUE_RECORD_09 = 9,
        ROGUE_RECORD_10 = 10,
        ROGUE_RECORD_11 = 11,
        ROGUE_RECORD_12 = 12,
        ROGUE_RECORD_13 = 13,
        ROGUE_RECORD_14 = 14,
        MAX = 15,
        INVALID = 16,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::EnemyActionCameraPlayRequestRoot {
    enum class ConstTargetType : int64_t {
        OwnerObject = 0,
        ActionTargetObject = 1,
    };
}
namespace app::ropeway::SteamPipeAttackUserDataToEnemy {
    enum class SteamPipeType : int64_t {
        Normal = 0,
        Special = 1,
        Notice = 2,
    };
}
namespace via::motion {
    enum class SequenceGetMode : int64_t {
        TypeA = 0,
        TypeB = 1,
    };
}
namespace via::wwise {
    enum class StoppedEventType : int64_t {
        None = 0,
        GameObject_EventId_Duration = 1,
        GameObject_RequestId_Duration = 2,
        RequestId_Duration = 3,
        Target_EventId_Duration = 4,
        Max = 5,
    };
}
namespace app::ropeway::enemy::em9999::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_RUN = 2,
        ACT_TURN = 3,
        ATK_ATTACK = 2000,
        ATK_JUMP_ATTACK = 2001,
        DMG_STUN = 3000,
        DIE_NORMAL = 5000,
        DIE_RIGID = 5001,
    };
}
namespace app::ropeway::enemy::em6300::BehaviorTreeAction {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_THREAT = 4,
        ATK_NAIL = 2000,
        ATK_NAIL2 = 2001,
        ATK_NAIL_BACK = 2002,
        ATK_THRUST = 2003,
        ATK_GARIPPER = 2005,
        ATK_POWER_DUNK = 2006,
    };
}
namespace app::ropeway::TimerBase {
    enum class CountMode : int64_t {
        Second = 0,
        Frame = 1,
    };
}
namespace app::ropeway::NaviMoveSupporter {
    enum class NavigationType : int64_t {
        Navigation = 0,
        WayPoint = 1,
        Direct = 2,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class DeadAction : int64_t {
        SLIP_DOWN_00 = 0,
        TUMBLE_CHAIR_00 = 1,
        OUTBREAK_FROM_LOCKER_00 = 2,
        HANG_HEAD_00 = 3,
    };
}
namespace app::ropeway::weapon::shell::ShellBase {
    enum class BulletShellStatusInformation : int64_t {
        NoSuspend = 0,
        Update = 1,
        LateUpdate = 2,
    };
}
namespace app::ropeway::gui::RogueInGameShopBehavior {
    enum class State : int64_t {
        INVALID = 0xFFFFFFFF,
        DEFAULT = 0,
    };
}
namespace app::ropeway::ObjectUpdateAndDrawRestrictManager {
    enum class RestrictType : int64_t {
        NONE = 0,
        LOADING = 1,
        MENU_PAUSE = 2,
        INVENTORY = 3,
        ITEM_DETAIL = 4,
        GIMMICK = 5,
        STOP_UPDATE = 6,
        TITLE = 7,
        TUTORIAL = 8,
        ALL = 9,
        DEBUG_PAUSE = 10,
        DEBUG_STOP_ENEMY = 11,
        CUT_SCENE = 12,
        GIMMICK_DEMO_PLEM = 13,
        GIMMICK_DEMO_EM_ONLY = 14,
        GIMMICK_MOVE_ACTOR = 15,
        GIMMICK_STOP_EFFECT = 16,
        EVENT_PAUSE = 17,
        MOVIE = 18,
        GIMMICK_USEITEM = 19,
        SAFE_LOAD_WAIT = 20,
        USER_DEVICE_PAIRING = 21,
        STOP_LEVEL = 22,
        NUM = 23,
    };
}
namespace app::ropeway::gamemastering::GimmickEventManager {
    enum class CantCallInventory : int64_t {
        FSM_ADDITEM = 0,
        FSM_SLEEP_INVENTORY = 1,
    };
}
namespace app::ropeway::StaffRollSingleData {
    enum class DataType : int64_t {
        COMPANY = 0,
        ORGANIZATION = 1,
        JOB = 2,
        NAME = 3,
        LOGO = 4,
        RIGHTS = 5,
    };
}
namespace via::network::session {
    enum class SearchKeyGroup : int64_t {
        None = 0,
        Develop = 1,
        QA = 2,
        Master = 3,
        Pub = 4,
        Event = 5,
        Custom = 6,
    };
}
namespace via {
    enum class PropKind : int64_t {
        AutoDetect = 0,
        Getter = 1,
        Setter = 2,
        ArrayGetter = 3,
        ArraySetter = 4,
        Event = 5,
    };
}
namespace via::timeline {
    enum class NodeType : int64_t {
        Unknown = 0,
        GameObject = 1,
        Component = 2,
        Folder = 3,
    };
}
namespace app::ropeway::environment::GameObjectSwitcher::OrderTarget {
    enum class Command : int64_t {
        Keep = 0,
        On = 1,
        Off = 2,
    };
}
namespace app::ropeway::enemy::tracks::Em7200WallColliderTrack {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Hit0 = 0,
        Hit1 = 1,
        Hit2 = 2,
        Hit3 = 3,
        Hit4 = 4,
        Hit5 = 5,
        Hit6 = 6,
        Hit7 = 7,
        Hit8 = 8,
        Hit9 = 9,
    };
}
namespace app::ropeway::gui::NewInventoryDetailBehavior {
    enum class PuzzleDemoState : int64_t {
        NoMove = 0,
        PortableSafeStartDemo = 1,
        PortableSafeWaitDemo = 2,
        PortableSafeAngle = 3,
        PortableSafePlayMotion = 4,
        PortableSafeWaitMotion = 5,
        CarKeyStartDemo = 6,
        CarKeyWaitDemo = 7,
    };
}
namespace via::navigation::map::Node {
    enum class SystemAttributes : int64_t {
        SystemAttribute_ModifiedChildren = 0,
        SystemAttribute_Junction = 1,
        SystemAttribute_DisableHybridTrace = 2,
        SystemAttribute_MultiSharedEdge = 3,
        SystemAttribute_HasExtraLink = 4,
        SystemAttributeBitNum = 32,
    };
}
namespace app::ropeway::SurvivorDefine::State {
    enum class UpperBodyAdd : int64_t {
    };
}
namespace via::navigation::Navigation {
    enum class NoMapAction : int64_t {
        NoMove = 0,
        Straight = 1,
    };
}
namespace app::ropeway::gamemastering::MainFlowManager {
    enum class SoundFirstBoot : int64_t {
        NORMAL_BOOT = 0,
        FIRST_BOOT = 1,
    };
}
namespace app::ropeway::TinyTimer {
    enum class Mode : int64_t {
        None = 0,
        Up = 1,
        Down = 2,
    };
}
namespace app::ropeway::fsmv2::condition::TransitionFrame {
    enum class CheckTimingType : int64_t {
        ALWAYS = 0,
        MOTION_END = 1,
        FRAME = 2,
    };
}
namespace app::ropeway::gui::SelectDifficultyBehavior {
    enum class EndType : int64_t {
        SELECTED = 0,
        CANCELED = 1,
    };
}
namespace app::ropeway::survivor::npc::NpcOrderDefine::CommonOrder {
    enum class SetConditionKind : int64_t {
        Set = 0,
        Reset = 1,
    };
}
namespace via::effect {
    enum class PolygonDivideDrawFlag : int64_t {
        None = 0,
        Select = 1,
        All = 2,
    };
}
namespace app::ropeway::enemy::EnemyWeaponDataUserData::WeaponValueInfoBase {
    enum class ShotgunShellType : int64_t {
        Center = 0,
        Slag = 1,
    };
}
namespace app::ropeway::gamemastering::ResetTitleFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        FADE_WAIT = 1,
        RESET_WAIT = 2,
        FINALIZE = 3,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace app::ropeway::enemy::em9999::BehaviorTreeAction {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_RUN = 2,
        ACT_TURN = 3,
        ATK_ATTACK = 2000,
        ATK_JUMP_ATTACK = 2001,
    };
}
namespace app::ropeway::enemy::tracks::Em7200ColliderTrack {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Press = 0,
        Damage = 1,
        Sensor_Touch = 2,
        Attack_ArmL = 3,
        Attack_ArmR = 4,
        Attack_SecondArmL = 5,
        Attack_SecondArmR = 6,
        DamageWeak = 7,
        Attack_Hold = 8,
        Damage_Eye_Leg = 9,
        Damage_Eye_Arm = 10,
        Damage_Eye_Back = 11,
        AttackBurst_L = 12,
        AttackBurst_R = 13,
        AttackToss_L = 14,
        AttackToss_R = 15,
        DrawControl = 16,
        Decal_AttackBurst_L = 17,
        Decal_AttackBurst_R = 18,
        Decal_AttackToss_L = 19,
        Decal_AttackToss_R = 20,
        Sensor_Gimmick = 21,
        Attack_Outside = 22,
        DeadPress = 23,
        WallPress = 24,
        AimTarget = 25,
    };
}
namespace app::ropeway::weapon::shell::BallisticSettingBase {
    enum class Performance : int64_t {
        PENETRATION = 1,
        GRENADE = 2,
        THROW = 4,
    };
}
namespace app::ropeway::weapon::shell::ShellBase {
    enum class BulletShellStatusInformation_ : int64_t {
        NoSuspend = 0,
        Update = 1,
        LateUpdate = 2,
    };
}
namespace app::ropeway::rogue::GimmickRogueRaccoonFigureManager {
    enum class RaccoonID : int64_t {
        Invalid = 0xFFFFFFFF,
        Lost_0 = 0,
        Lost_1 = 1,
        Lost_2 = 2,
        Lost_3 = 3,
        Lost_4 = 4,
        Lost_5 = 5,
        Lost_6 = 6,
        Lost_7 = 7,
        Lost_8 = 8,
        Lost_9 = 9,
        Lost_10 = 10,
        Lost_11 = 11,
        Lost_12 = 12,
        Lost_13 = 13,
        Lost_14 = 14,
        Lost_15 = 15,
        Lost_16 = 16,
        Lost_17 = 17,
        Lost_18 = 18,
        Lost_19 = 19,
        NUM = 20,
    };
}
namespace app::ropeway::enemy::em7300::BehaviorTreeAction {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_BACKSTEP = 2,
        ACT_REACTION = 3,
        ATK_BACK = 2000,
        ATK_DASH = 2001,
        ATK_LANE_L = 2002,
        ATK_LANE_R = 2003,
        ATK_HIGH_R = 2004,
        ATK_MIDL_TWO = 2005,
        ATK_RAGE = 2006,
        ATK_RAGE_2 = 2007,
        ATK_BOTH_HAND = 2008,
        ATK_BATTLE_START = 2009,
    };
}
namespace via::navigation::map::Link {
    enum class PortalType : int64_t {
        Destination = 0,
        Source = 1,
    };
}
namespace System {
    enum class TypeCode : int64_t {
        Empty = 0,
        Object = 1,
        DBNULL_ = 2,
        Boolean = 3,
        Char = 4,
        SByte = 5,
        Byte = 6,
        Int16 = 7,
        UInt16 = 8,
        Int32 = 9,
        UInt32 = 10,
        Int64 = 11,
        UInt64 = 12,
        Single = 13,
        Double = 14,
        Decimal = 15,
        DateTime = 16,
        String = 18,
    };
}
namespace via::movie {
    enum class PlaybackStrategy : int64_t {
        OneShot = 0,
        Loop = 1,
        Replay = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickDoorOptions {
    enum class OptionType : int64_t {
        NOB_SPADE = 0,
        NOB_HEART = 1,
        NOB_DIAMOND = 2,
        NOB_CLUB = 3,
        CHAIN_ = 4,
        INSIDE_LOCK_SIMPLE_BOLT = 5,
        NOB_WASTE_WATER = 6,
        NOB_GARDEN = 7,
        NOB_CONVENIENCE_STORE = 8,
        CHAIN_HOLE = 9,
        CHAIN_NOB = 10,
        NOB_ORPHAN_ASYLUM = 11,
        NOB_LABORATORY = 12,
        NOB_ROGUE_OLD_KEY = 13,
        MAX = 14,
    };
}
namespace via::physics::CastRayQuery {
    enum class Type : int64_t {
        Unknown = 0,
        Ray = 1,
        EndPoint = 2,
        Max = 3,
    };
}
namespace via::motion {
    enum class MotionType : int64_t {
        None = 0,
        Motion = 1,
        Tree = 2,
    };
}
namespace app::ropeway::CharacterHandler {
    enum class SkipId : int64_t {
        None = 0,
        SkipId_00 = 1,
        SkipId_01 = 2,
        SkipId_02 = 4,
        SkipId_03 = 8,
        SkipId_04 = 16,
        SkipId_05 = 32,
        SkipId_06 = 64,
        SkipId_07 = 128,
        SkipId_08 = 256,
        SkipId_09 = 512,
        SkipId_10 = 1024,
        SkipId_11 = 2048,
        SkipId_12 = 4096,
        SkipId_13 = 8192,
        SkipId_14 = 16384,
        SkipId_15 = 32768,
        SkipId_16 = 65536,
        SkipId_17 = 131072,
        SkipId_18 = 262144,
        SkipId_19 = 524288,
        SkipId_20 = 1048576,
        SkipId_21 = 2097152,
        SkipId_22 = 4194304,
        SkipId_23 = 8388608,
        SkipId_24 = 16777216,
        SkipId_25 = 33554432,
        SkipId_26 = 67108864,
        SkipId_27 = 134217728,
        SkipId_28 = 268435456,
        SkipId_29 = 536870912,
        SkipId_30 = 1073741824,
        SkipId_31 = 2147483648,
    };
}
namespace app::ropeway::gui::RogueInventorySlotBehavior {
    enum class FatSlotPartsA : int64_t {
        sm71_903 = 50,
        sm71_909 = 56,
        sm71_913 = 60,
        sm71_915 = 62,
        sm71_917 = 64,
    };
}
namespace app::ropeway::gimmick::action::GimmickRogueEnteranceGate {
    enum class ArgNo : int64_t {
        Open = 0,
        Wait = 1,
    };
}
namespace via::motion::detail::ChainLinkData {
    enum class LinkMode : int64_t {
        TopToBottom = 0,
        BottomToTop = 1,
        Manual = 2,
    };
}
namespace app::ropeway::enemy::em9000::Em9000Think {
    enum class KeySeTypeEnum : int64_t {
        HAVE = 0,
        DONTHAVE = 1,
    };
}
namespace app::ropeway {
    enum class FootIndex : int64_t {
        LEFT = 0,
        RIGHT = 1,
        BACK_LEFT = 0,
        BACK_RIGHT = 1,
        FORE_LEFT = 2,
        FORE_RIGHT = 3,
        MAX = 4,
    };
}
namespace via::gui::MessageAnalyzer {
    enum class State : int64_t {
        Normal = 0,
        Ruby = 1,
        RubyRB = 2,
        RubyRT = 3,
    };
}
namespace app::ropeway {
    enum class CERO : int64_t {
        None = 0,
        D = 1,
        Z = 2,
    };
}
namespace via::physics::RequestSetColliderResource {
    enum class RequestSetState : int64_t {
        AlwaysRegister = 0,
        Max = 1,
    };
}
namespace System::Collections::Specialized {
    enum class NotifyCollectionChangedAction : int64_t {
        Add = 0,
        Remove = 1,
        Replace = 2,
        Move = 3,
        Reset = 4,
    };
}
namespace app::ropeway::enemy::em7100 {
    enum class CompareType : int64_t {
        EQ = 0,
        NE = 1,
        GE = 2,
        GT = 3,
        LE = 4,
        LT = 5,
    };
}
namespace app::ropeway::fsmv2::DispDialog {
    enum class EndResult : int64_t {
        SUSPENSE = 0,
        YES = 1,
        NO = 2,
    };
}
namespace app::ropeway::enemy::em5000::fsmv2::condition::Em5000FsmCondition_CheckIvyChainBlend {
    enum class CheckType : int64_t {
        ZERO = 0,
        ONE = 1,
    };
}
namespace app::ropeway::fsmv2::RestrictRoguePlayerAction {
    enum class RestrictType : int64_t {
        Start = 0,
        End = 1,
    };
}
namespace via::hid::camera {
    enum class PlayStationCameraFrameStatus : int64_t {
        Active = 0,
        NotActive = 1,
        AlreadyRead = 2,
        NotStable = 3,
        InvalidFrame = 4,
        InvalidMetaData = 5,
    };
}
namespace app::ropeway::fsmv2::enemy::em7000 {
    enum class ATK_BREAK_TANK : int64_t {
        Break = 3738789812,
    };
}
namespace app::ropeway::gimmick::action::MaterialParamAnimator {
    enum class AnimationType : int64_t {
        NONE = 0,
        SIN_CURVE = 1,
        BLINK = 2,
        DISC_ACCESS = 3,
    };
}
namespace app::ropeway::enemy::em5000::MotionPattern {
    enum class PivotTurn : int64_t {
        L90 = 0,
        R90 = 1,
        L180 = 2,
        R180 = 3,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class FakeDeadActionType : int64_t {
        WAKEUP = 0,
        HOLD = 1,
    };
}
namespace via::effect::detail {
    enum class StretchType : int64_t {
        Time = 0,
        Distance = 1,
    };
}
namespace app::ropeway::gui::ItemCurvePuzzleBehavior {
    enum class VarName : int64_t {
        LineA_Scroll_PL = 0,
        LineA_Height_PL = 1,
        LineA_WaveLength_PL = 2,
        LineB_Scroll_PL = 3,
        LineB_Height_PL = 4,
        LineB_WaveLength_PL = 5,
        LineA_Scroll_Set = 6,
        LineA_Height_Set = 7,
        LineA_WaveLength_Set = 8,
        LineB_Scroll_Set = 9,
        LineB_Height_Set = 10,
        LineB_WaveLength_Set = 11,
        MAX = 12,
    };
}
namespace app::ropeway::WwiseOptionMenu {
    enum class DynamicRangeControl : int64_t {
        Small = 0,
        Large = 1,
    };
}
namespace app::ropeway::ChoreographPlayer::ChoreographParamCurveAnimator {
    enum class WaitType : int64_t {
        DISABLE = 0,
        RATE_0 = 1,
    };
}
namespace app::ropeway::fsmv2::GimmickValueCheck {
    enum class ActioinType : int64_t {
        ActionEnd = 0,
        ActionFailed = 1,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::EnemyActionCameraPlayRequestRoot {
    enum class BankTypeSelectType : int64_t {
        SET_BANKTYPE = 0,
        USE_MAPPING_DATA = 1,
    };
}
namespace app::ropeway::enemy::em6300::fsm::tag {
    enum class StateAttr : int64_t {
        Walk = 1009902001,
        Dash = 3584463383,
        DashStagger = 273777789,
        LookAt = 3968314242,
        AttackDisable = 50910426,
        ToWalk = 1975550168,
    };
}
namespace via::movie {
    enum class Codec : int64_t {
        Unknown = 0,
        H264 = 1,
        WMV9 = 2,
        LLMV_SLP = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickElectricStorageForG2Battle {
    enum class RoutineNo : int64_t {
        Wait = 0,
        Fall = 1,
        Attack = 2,
        WaitAfterAttack = 3,
    };
}
namespace app::ropeway::fsmv2::Wait {
    enum class WaitTypeEnum : int64_t {
        Normal = 0,
    };
}
namespace app::ropeway::AttackUserDataToEnemy {
    enum class Hit : int64_t {
        Normal = 0,
        Critical = 1,
    };
}
namespace app::ropeway::enemy::em9000::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_RUN = 2,
        ACT_TURN = 3,
        ACT_TURN_L180 = 4,
        ACT_TURN_R180 = 5,
        ACT_TURN_L90 = 6,
        ACT_TURN_R90 = 7,
        ACT_FOUND = 8,
        ACT_PAIN_RUN = 9,
        ACT_CURTAIN_ = 10,
        ACT_NOLIGHT_RUN = 11,
        ACT_NOLIGHT_RUN_SLOW = 12,
        ACT_WASH = 13,
        ACT_BEDROOM_00 = 14,
        ACT_BEDROOM_01 = 15,
        ACT_BEDROOM_02 = 16,
        ACT_AXE_RUN = 17,
        GMK_DOOR_OPEN_UNLOCK_OPEN = 1000,
        GMK_DOOR_OPEN_CLOSE_LOCK = 1001,
        GMK_DOOR_OPEN_AXE_BREAK = 1002,
        GMK_DOOR_OPEN_FAST = 1003,
        GMK_DOOR_OPEN_BEDROOM = 1004,
        GMK_DOOR_OPEN_1ST = 1005,
        GMK_DOOR_WASHEND = 1006,
        GMK_DOOR_WASHEND_ANGER = 1007,
        ATK_ATTACK = 2000,
        ATK_GRAPPLE_DEAD = 2001,
        ATK_GRAPPLE_DEAD_F = 2002,
        ATK_GRAPPLE_DEAD_UL = 2003,
        ATK_GRAPPLE_DEAD_UR = 2004,
        DMG_STUN = 3000,
        DIE_NORMAL = 5000,
        DIE_RIGID = 5001,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace via::motion::detail {
    enum class InstancePoolType : int64_t {
        DoNothing = 0,
        CallFinalize = 1,
        PlacementNew = 2,
    };
}
namespace via::hid::camera::detail::PlayStationCameraLedTracker {
    enum class StartResult : int64_t {
        OK = 0,
        ErrorUnknown = 0xFFFFFFFF,
        ErrorAlreadyStarted = -2,
        ErrorUpdateStatusIsNotWait = -3,
        ErrorCameraDeviceIsNotFound = -4,
        ErrorCameraStartFailed = -5,
        ErrorNotSupported = -6,
    };
}
namespace app::ropeway::enemy::em7000::Em7000Define {
    enum class AttackId : int64_t {
        ATTACK_ID_NONE = 0,
        ATTACK_ID_WAIT_BATTLE = 1,
        ATTACK_ID_W_SHORT_R = 2,
        ATTACK_ID_W_CONTINUOUS_2 = 3,
        ATTACK_ID_W_CONTINUOUS_3 = 4,
        ATTACK_ID_W_MIDDLE = 5,
        ATTACK_ID_G_SHORT_R = 6,
        ATTACK_ID_G_SHORT_F = 7,
        ATTACK_ID_G_CONTINUOUS_MIDDLE = 8,
        ATTACK_ID_TO_G = 9,
        ATTACK_ID_TO_W = 10,
        ATTACK_ID_GRAPPLE = 11,
        ATTACK_ID_W_LARGE_A = 12,
        ATTACK_ID_W_CONTINUOUS_A = 13,
        ATTACK_ID_W_CONTINUOUS_B = 14,
        ATTACK_ID_W_COUTINUOUS_WALK = 15,
        ATTACK_ID_G_SHORT_R_FORWARD = 16,
        ATTACK_ID_G_SHORT_F_FORWARD = 17,
        ATTACK_ID_G_LARGE_A = 18,
        ATTACK_ID_START_LEON = 19,
        ATTACK_ID_G_WALK_ATTACK = 20,
        ATTACK_ID_G_PUSH_ATTACK = 21,
        ATTACK_ID_W_GARPPLE = 22,
        ATTACK_ID_BATTLE_START = 23,
    };
}
namespace via::dynamics {
    enum class ShapeType : int64_t {
        Invalid = 0,
        Sphere = 1,
        Capsule = 2,
        Box = 3,
        Mesh = 4,
        Triangle = 5,
        ConvexHull = 6,
        Max = 7,
    };
}
namespace via::hid::AudioInManager {
    enum class AUDIO_IN_TYPE : int64_t {
        NORMAL = 0,
        VOICE_CHAT = 1,
        VOICE_RECOGNITION = 2,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class Situation : int64_t {
        NORMAL = 0,
        WATER = 1,
        LIGHT = 2,
        COMBAT = 3,
        TENSION = 4,
    };
}
namespace app::ropeway::gui::SubTitleBehavior {
    enum class Type : int64_t {
        ADVENTURE = 0,
        CONV = 1,
        CONV_DEADEND = 2,
        EVENT = 3,
    };
}
namespace via::motion::IkDog {
    enum class RayCastSkipOption : int64_t {
        None = 0,
        DIV2 = 2,
        DIV4 = 4,
    };
}
namespace via::str::detail::format::boost::format_item {
    enum class ExFmtFlag : int64_t {
        Ignore = 0,
        SetPrecision = 0,
        ZeroPadding = 0,
        Centering = 0,
    };
}
namespace app::ropeway::gamemastering::TimelineEventManager {
    enum class EventIDOffset : int64_t {
        CF_START = 100000,
        EV_START = 200000,
    };
}
namespace app::ropeway::gimmick::option::VibrationSettings::Param {
    enum class VibSize : int64_t {
        NONE = 0,
        SS = 1,
        S = 2,
        M = 3,
        L = 4,
    };
}
namespace app::ropeway::weapon::fsmv2::action::WeaponFireAction {
    enum class TimingType : int64_t {
        OnStart = 0,
        OnUpdate = 1,
        OnEnd = 2,
    };
}
namespace app::ropeway::StaticCompoundController {
    enum class Phase : int64_t {
        Initial = 0,
        GameObjectCollection = 1,
        GameObjectSubFolderCollection = 2,
        Bake = 3,
        Finish = 4,
    };
}
namespace via::storage {
    enum class SaveServiceWriteMode : int64_t {
        SaveServiceWriteMode_Default = 131072,
        SaveServiceWriteMode_1_Byte = 1,
        SaveServiceWriteMode_512_Sector = 512,
        SaveServiceWriteMode_1_KiloByte = 1024,
        SaveServiceWriteMode_4K_Sector = 4096,
        SaveServiceWriteMode_64_KiloByte = 65536,
        SaveServiceWriteMode_128_KiloByte = 131072,
        SaveServiceWriteMode_256_KiloByte = 262144,
        SaveServiceWriteMode_512_KiloByte = 524288,
        SaveServiceWriteMode_1_MegaByte = 1048576,
        SaveServiceWriteMode_HighSpeed = 2147483647,
    };
}
namespace app::ropeway::gamemastering::TimelineEventManager {
    enum class EventFadeEnable : int64_t {
        TRUE_ = 0,
        FALSE_ = 1,
    };
}
namespace via::gui {
    enum class GridScrollDirection : int64_t {
        None = 0,
        Up = 1,
        Right = 2,
        Down = 3,
        Left = 4,
    };
}
namespace app::ropeway::enemy::em6200::tracks::Em6200HoldDamageTrack {
    enum class HoldDamageType : int64_t {
        THROW = 0,
    };
}
namespace app::ropeway::gimmick::GimmickIconMessenger {
    enum class RequestType : int64_t {
        Interact = 0,
        Update = 1,
        Remove = 2,
        Multipurpose = 3,
    };
}
namespace via::motion::DataHeader {
    enum class ErrFlags : int64_t {
        None = 0,
        Empty = 1,
        NotFoundRefAsset = 2,
        NotFoundIncludeAsset = 4,
    };
}
namespace app::ropeway::enemy::em4000::fsmv2::condition::TargetDirection {
    enum class CompareType : int64_t {
        Equal = 0,
        NotEqual = 1,
        Less = 2,
        LessEq = 3,
        Greater = 4,
        GreaterEq = 5,
    };
}
namespace via {
    enum class SystemServicePerformanceMode : int64_t {
        Invalid = 0,
        Normal = 1,
        Boost = 2,
    };
}
namespace app::ropeway::enemy::em5000 {
    enum class WeakPartsSize : int64_t {
        Small = 0,
        Medium = 1,
        Large = 2,
    };
}
namespace app::ropeway::gui::GameOverSceneTimelineBehavior {
    enum class GameOverType : int64_t {
        DEAD = 0,
        FAINTED = 1,
        MAX = 2,
    };
}
namespace via::network::AutoMatchmaking {
    enum class DataType : int64_t {
        Number = 0,
        TotalValue = 1,
        TotalBit = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickBreakableColumnForG2Battle {
    enum class Routine : int64_t {
        Initial = 0,
        Break = 1,
        Break_Breaking = 2,
        Break_Sleep = 3,
    };
}
namespace app::ropeway::gamemastering::MainFlowManager {
    enum class MainState : int64_t {
        INVALID = 0,
        INITIALIZE = 1,
        WAKE_UP = 2,
        TITLE = 3,
        LOAD_GAME_DATA = 4,
        IN_GAME_INITIALIZE = 5,
        IN_GAME = 6,
        GAME_OVER = 7,
        GAME_OVER_TO_RESET_TO_WAKE_UP = 8,
        PAUSE = 9,
        STAFFROLL = 10,
        STAFFROLL_TO_RESET_TO_WAIT = 11,
        WAIT_STAFFROLL = 12,
        ENDING = 13,
        RESULT = 14,
        RESULT_EXTRA = 15,
        RESET_TO_WAKE_UP = 16,
        RESET_TO_TITLE = 17,
        RESET_TO_GAME = 18,
        RESET_TO_STAFFROLL = 19,
        RESET_TO_WAIT_STAFFROLL = 20,
        RESET_TO_RESULT = 21,
        RESET_TO_RESULT_EXTRA = 22,
        RESET_TO_EXTRAMENU = 23,
        RESET_TO_EXTRA_CONTINUE_4 = 24,
        RESET_TO_EXTRA_CONTINUE_T = 25,
        RESET_TITLE_TO_WAKE_UP = 26,
        RESET_TO_ROGUE_BASE = 27,
        RESET_TO_ROGUE_IN_GAME = 28,
        ROGUE_BASEAREA = 29,
        ROGUE_RESULT = 30,
        LMODE_SETUP = 31,
        RESET_TO_LMODE_INGAME = 32,
        RESET_TO_RESULT_ROGUE = 33,
        FORCE_TITLE_VS = 34,
        DEVELOP_INITIALIZE = 35,
        DEVELOP_LOAD_WAIT = 36,
        DEVELOP_FORCE_RESET_INGAME = 37,
        DEVELOP_SCENARIO_JUMP = 38,
    };
}
namespace app::ropeway::gui::ThankYouBehavior {
    enum class State : int64_t {
        INITIALIZE = 0,
        UPDATE = 1,
        GUI_WAIT = 2,
    };
}
namespace app::ropeway::enemy::em5000::fsmv2::action::Em5000FsmAction_SetChainBlendRate {
    enum class SettingType : int64_t {
        KEEP = 0,
        SET = 1,
        RESET = 2,
    };
}
namespace app::ropeway::weapon::generator::BombGeneratorUserDataBase {
    enum class BombRocketGenerateSetting : int64_t {
        S = 0,
        M = 1,
        L = 2,
    };
}
namespace via::geometry {
    enum class BrushPipelineState : int64_t {
        Shape = 0,
        ShapeDefault = 1,
        ShapeShadow = 2,
        ShapeWire = 3,
        ShapePick = 4,
        ImagePlane = 5,
        ImagePlaneDefault = 6,
        ShapeTransparent = 7,
        ShapeDefaultTransparent = 8,
        ImagePlaneTransparent = 9,
        ImagePlaneDefaultTransparent = 10,
        Max = 11,
    };
}
namespace app::ropeway::gui::OptionBehavior {
    enum class ItemMode : int64_t {
        SELECTOR = 0,
        SELECTOR_HDRMODE = 1,
        SELECTOR_WINDOWMODE = 2,
        SELECTOR_DIRECTX = 3,
        SELECTOR_RENET = 4,
        SELECTOR_PC = 5,
        SLIDER = 6,
        EXECUTER = 7,
        EXECUTER2 = 8,
        EXECUTER_PC = 9,
    };
}
namespace app::ropeway {
    enum class BehaviorTreeCategory : int64_t {
        Invalid = 0xFFFFFFFF,
        BehaviorTree = 0,
        MotionFsm = 1,
        MotionJackFsm = 2,
    };
}
namespace app::ropeway::gui::FloorMapSt4ABehavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_4_717_0 = 1,
        Room_4_755_0 = 2,
    };
}
namespace app::ropeway::gui::RogueInventoryBehavior {
    enum class TabMode : int64_t {
        Map = 0,
        Item = 1,
        File = 2,
    };
}
namespace app::ropeway::fsmv2::CharacterControllerSwitch {
    enum class SwitchType : int64_t {
        ON = 0,
        OFF = 1,
    };
}
namespace app::ropeway::ReNetData::OtherData {
    enum class OtherDataType : int64_t {
        Normal = 0,
        Count = 1,
        Distance = 2,
        Timer = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickWristTagReader {
    enum class RTTRoutineType : int64_t {
        NEED = 0,
        ATTACHED = 1,
        DETACHED = 2,
        ERROR_ = 3,
    };
}
namespace via::autoplay::action::AutoAttack {
    enum class Precision : int64_t {
        High = 0,
        Medium = 1,
        Low = 2,
    };
}
namespace app::ropeway::posteffect::RenderOutputController {
    enum class ContactShadowLimit : int64_t {
        NoLimited = 0,
        Limited = 1,
        ReleaseCheck = 2,
    };
}
namespace app::ropeway::behaviortree::condition::survivor::npc::NpcBtCondition_OnPlayerRay {
    enum class CheckType : int64_t {
        START_CHECK = 0,
        END_CHECK = 1,
    };
}
namespace via::render {
    enum class UsageType : int64_t {
        Default = 0,
        Immutable = 1,
        Dynamic = 2,
        Staging = 3,
    };
}
namespace app::ropeway::WeaponDefine {
    enum class BulletDirType : int64_t {
        WEAPON = 0,
        CAMERA = 1,
    };
}
namespace via::effect::gpgpu::detail {
    enum class DepthOperatorFlags : int64_t {
        UseBounce = 1,
        UseYNormalEmission = 2,
    };
}
namespace via::network::wrangler {
    enum class ProviderEnabledState : int64_t {
        Undefined = 0,
        ForceOff = 1,
        OffByDefault = 2,
        OnByDefault = 3,
        ForceOn = 4,
    };
}
namespace app::ropeway::gamemastering::UserDeviceMonitoringFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        UPDATE = 1,
        DIALOG = 2,
        FINALIZE = 3,
        FOR_PLATFORM_FIRST_INPUT = 4,
        CHECK_INPUT = 5,
        CHECK_ACCOUNT_PICKER = 6,
        SHOW_ACCOUNT_PICKER = 7,
        CHECK_REBOOT = 8,
        WAIT_REBOOT = 9,
        REBOOT = 10,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace via::render {
    enum class HazeCompositorBlur : int64_t {
        Level1 = 0,
        Level2 = 1,
        Level3 = 2,
        Level4 = 3,
    };
}
namespace via::navigation::Navigation {
    enum class NoNodeSearch : int64_t {
        None = 0,
        NearestNode = 1,
    };
}
namespace via::hid {
    enum class GestureState : int64_t {
        BegIN_ = 0,
        Update = 1,
        End = 2,
        Canceled = 3,
        BeginOneShot = 4,
    };
}
namespace app::ropeway::ReNetData::ExtraProgressData {
    enum class ProgreeeType : int64_t {
        Hunk = 0,
        Tofu_A = 1,
        Tofu_B = 2,
        Tofu_C = 3,
        Tofu_D = 4,
        Tofu_E = 5,
        LOST_A = 6,
        LOST_B = 7,
        LOST_C = 8,
        LOST_D = 9,
    };
}
namespace app::ropeway::gimmick::action::GimmickDoorOptionSetupParam {
    enum class SideType : int64_t {
        BOTH = 0,
        ONLY_A = 1,
        ONLY_B = 2,
    };
}
namespace app::ropeway::survivor::npc::NpcParam {
    enum class WaitType : int64_t {
        JOG_START = 0,
        JOG_TO_WALK = 1,
        ANY = 2,
    };
}
namespace via::hid {
    enum class NpadId : int64_t {
        Handheld = 1,
        No1 = 2,
        No2 = 4,
        No3 = 8,
        No4 = 16,
        No5 = 32,
        No6 = 64,
        No7 = 128,
        No8 = 256,
        Invalid = 0xFFFFFFFF,
    };
}
namespace app::ropeway::enemy::em5000::fsmv2::action::Em5000FsmAction_SetMotionPattern {
    enum class SetMethodType : int64_t {
        DIRECT = 0,
        RANDOM = 1,
        LAST_DAMAGE_FB = 2,
        LAST_DAMAGE_LR = 3,
        POSTURE = 4,
    };
}
namespace via::physics {
    enum class PhaseType : int64_t {
        AfterUpdate = 0,
        CharacterControl = 1,
        AfterLateUpdate = 2,
        Max = 3,
    };
}
namespace via::effect::gpgpu::detail {
    enum class GpuParentOptions : int64_t {
        None = 0,
        UseLocal = 4,
    };
}
namespace app::ropeway::gamemastering::Location {
    enum class ID : int64_t {
        invalid = 0,
        CityArea = 1,
        Factory = 2,
        Laboratory = 3,
        MountaIN_ = 4,
        Opening = 5,
        Orphanage = 6,
        Police = 7,
        SewagePlant = 8,
        Sewer = 9,
        Playground = 10,
        DLC_Laboratory = 11,
        DLC_Aida = 12,
        DLC_Hunk = 13,
        Opening2 = 14,
        GasStation = 15,
        RPD = 16,
        WasteWater = 17,
        WaterPlant = 18,
        EV011 = 19,
        EV050 = 20,
        LaboratoryUndermost = 21,
        Transportation = 22,
        GasStation2 = 23,
        OrphanAsylum = 24,
        OrphanApproach = 25,
        CrocodiliaArea = 26,
        Title = 27,
        Movie = 28,
        RPD_B1 = 29,
        Opening3 = 30,
        GameOver = 31,
        Result = 32,
        Ending = 33,
        LOCATION_NUM = 34,
    };
}
namespace via::behaviortree::action::SetBool {
    enum class Status : int64_t {
        FALSE_ = 0,
        TRUE_ = 1,
    };
}
namespace app::ropeway::ObjectUpdateAndDrawRestrictManager::GimmickInUseInfo {
    enum class UserType : int64_t {
        ENEMY = 0,
        PLAYER = 1,
    };
}
namespace app::ropeway::enemy::em6200::fsmv2::action::Em6200FsmAction_CheckCombo {
    enum class CHECK_BIT : int64_t {
        Punch = 0,
        Hammer = 1,
    };
}
namespace via::render::command::UpdateBuffer {
    enum class BufferType : int64_t {
        VertexBuffer = 0,
        IndexBuffer = 1,
        Buffer = 2,
        VertexBufferFromUserPtr = 3,
        IndexBufferFromUserPtr = 4,
        BufferFromUserPtr = 5,
    };
}
namespace app::ropeway::enemy::em6000::MotionPattern {
    enum class Stun : int64_t {
        F = 0,
        B = 1,
        L = 2,
        R = 3,
    };
}
namespace via::storage::saveService {
    enum class SaveDataType : int64_t {
        Array = 0xFFFFFFFF,
        Int32 = 0,
        Int64 = 1,
        Float = 2,
        Struct = 3,
        String = 4,
        Class = 5,
    };
}
namespace via::wwise {
    enum class SinkTypeWindows : int64_t {
        MaIN_ = 0,
        MergeToMaIN_ = 1,
        Secondary = 2,
        None = 3,
    };
}
namespace via::render {
    enum class SrvDimension : int64_t {
        Unknown = 0,
        Buffer = 1,
        Texture1d = 2,
        Texture1darray = 3,
        Texture2d = 4,
        Texture2darray = 5,
        Texture2dms = 6,
        Texture2dmsarray = 7,
        Texture3d = 8,
        Texturecube = 9,
        Texturecubearray = 10,
        Bufferex = 11,
    };
}
namespace app::ropeway::weapon::shell::ShellDefine {
    enum class FireBulletType : int64_t {
        Camera = 0,
        Muzzle = 1,
        PossiblyMuzzle = 2,
        AlongMuzzle = 3,
        None = 4,
    };
}
namespace app::ropeway::GimmickOptionDoorSettings {
    enum class OptionType : int64_t {
        INSIDE_LOCK_NODISP = 0,
        INSIDE_LOCK_BARRED = 1,
        MAX = 2,
    };
}
namespace via::render::CubeTo2D {
    enum class TYPE : int64_t {
        PARABOROID = 0,
        SPHERE = 1,
    };
}
namespace via {
    enum class AccountPickerShowRequestResult : int64_t {
        Success = 0,
        ErrorNotSupported = 1,
        ErrorAlreadyShown = 2,
        ErrorNoDevices = 3,
        ErrorInvalidParam = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickElevator {
    enum class ElevatorShapeType : int64_t {
        BOX = 0,
        ROUND = 1,
    };
}
namespace app::ropeway::TerrainAvoidanceLink {
    enum class CollisionFilterType : int64_t {
        TerraIN_ = 0,
        Effect = 1,
        Dynamics = 2,
    };
}
namespace app::ropeway::enemy::em7300::tracks::Em7300WallLRTrack {
    enum class LRType : int64_t {
        WALL_L = 0,
        WALL_R = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickLadder {
    enum class MotionType : int64_t {
        START = 0,
        LOOP = 1,
        END = 2,
        MAX = 3,
    };
}
namespace via::motion::TransitionData {
    enum class StartType : int64_t {
        Frame = 0,
        NormalizedTime = 1,
        SyncTime = 2,
        AutoSyncTime = 3,
        AutoSyncTimeSamePointCount = 4,
    };
}
namespace via::effect::script::EPVDefine {
    enum class AttackTypeEnum : int64_t {
        Slash = 96,
        Stab = 97,
        Shoot = 98,
        Strike = 99,
        Bite = 100,
        Explosion = 102,
        Special = 111,
    };
}
namespace via::network::BattleReplay {
    enum class Error : int64_t {
        None = 0,
        Abort = 1,
    };
}
namespace app::ropeway::gamemastering::InventoryManager {
    enum class STOCK_TYPE : int64_t {
        BLANK = 0,
        ITEM = 1,
        WEAPON = 2,
        DEAD = 3,
    };
}
namespace via::render {
    enum class ShallowWaterRenderingMode : int64_t {
        Translucent = 0,
        TranslucentLighting = 1,
        GBuffer = 2,
        GBufferDepth = 3,
        UserMaterial = 4,
    };
}
namespace via::effect::script::EffectPositionHolder::PositionInfo {
    enum class Status : int64_t {
        Start = 0,
        Update = 1,
        End = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0000_MONTAGE : int64_t {
        ID000 = 0,
        ID001 = 1,
        ID002 = 2,
        ID003 = 3,
        ID004 = 4,
        ID005 = 5,
        ID006 = 6,
        ID007 = 7,
        ID008 = 8,
        ID009 = 9,
        ID010 = 10,
        ID011 = 11,
        ID012 = 12,
        ID013 = 13,
        ID014 = 14,
        ID015 = 15,
        ID016 = 16,
        ID017 = 17,
        ID018 = 18,
        ID019 = 19,
        ID020 = 20,
        ID021 = 21,
        ID022 = 22,
        ID023 = 23,
        ID024 = 24,
        ID025 = 25,
        ID026 = 26,
        ID070 = 27,
        ID071 = 28,
        ID072 = 29,
        ID073 = 30,
        ID074 = 31,
        ID200 = 32,
        ID201 = 33,
        ID202 = 34,
        ID203 = 35,
        ID204 = 36,
        ID205 = 37,
        ID206 = 38,
        ID207 = 39,
        ID208 = 40,
        ID209 = 41,
        ID210 = 42,
        ID211 = 43,
        ID212 = 44,
        ID213 = 45,
        ID214 = 46,
        ID215 = 47,
        ID216 = 48,
        ID270 = 49,
        ID271 = 50,
        ID300 = 51,
        ID301 = 52,
        ID302 = 53,
        ID303 = 54,
        ID304 = 55,
        ID305 = 56,
        ID400 = 57,
        ID401 = 58,
        ID402 = 59,
        ID403 = 60,
        ID404 = 61,
        ID470 = 62,
        ID570 = 63,
        ID777 = 64,
        ID800 = 65,
        ID801 = 66,
        ID802 = 67,
        ID803 = 68,
        ID804 = 69,
        ID805 = 70,
        ID806 = 71,
        ID807 = 72,
        ID808 = 73,
        ID809 = 74,
        ID810 = 75,
        ID811 = 76,
        ID900 = 77,
        ID901 = 78,
        ID902 = 79,
        ID903 = 80,
        ID910 = 81,
        ID911 = 82,
        ID912 = 83,
        ID913 = 84,
        ID914 = 85,
        ID920 = 86,
        ID921 = 87,
        ID922 = 88,
        ID923 = 89,
        ID924 = 90,
        ID925 = 91,
        ID926 = 92,
        ID927 = 93,
        ID928 = 94,
        ID929 = 95,
        ID930 = 96,
        ID931 = 97,
        ID932 = 98,
        ID933 = 99,
        ID999 = 100,
    };
}
namespace via::effect::script::EPVExpertFootLandingData {
    enum class FootStepType : int64_t {
        Contact = 0,
        Lift = 1,
        Step = 2,
        Slide = 3,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class LiftArmKindID : int64_t {
        TYPE_A = 0,
        TYPE_B = 1,
        TYPE_C = 2,
        TYPE_D = 3,
    };
}
namespace via::effect::script {
    enum class PropID : int64_t {
        Etc = 0,
    };
}
namespace app::ropeway::gimmick::option::EnemyControlSettings::EnemyControlParam {
    enum class SetTargetGround : int64_t {
        Ground = 0,
        Wall = 1,
        Ceiling = 2,
    };
}
namespace app::ropeway::enemy::userdata::Em4000AttackUserData {
    enum class AttackPatternType : int64_t {
        Hold = 0,
    };
}
namespace app::ropeway::enemy::EnemyWeaponDataUserData::WeaponValueInfoBase {
    enum class KnifeShellType : int64_t {
        Normal = 0,
        HoldEscape = 1,
    };
}
namespace app::ropeway::gimmick::option::MaterialParamSettings::Param {
    enum class SettingType : int64_t {
        Emissive = 0,
        EmissiveColor = 1,
    };
}
namespace via::clr {
    enum class ElementType : int64_t {
        End = 0,
        Void = 1,
        Boolean = 2,
        Char = 3,
        I1 = 4,
        U1 = 5,
        I2 = 6,
        U2 = 7,
        I4 = 8,
        U4 = 9,
        I8 = 10,
        U8 = 11,
        R4 = 12,
        R8 = 13,
        String = 14,
        Ptr = 15,
        ByRef = 16,
        ValueType = 17,
        Class = 18,
        Var = 19,
        Array = 20,
        GenericInst = 21,
        TypedByRef = 22,
        I = 24,
        U = 25,
        FNPtr = 27,
        Object = 28,
        SzArray = 29,
        MVar = 30,
        RE_Reqd = 31,
        RE_Opt = 32,
        Internal = 33,
        Max = 34,
        Modifier = 64,
        Stencil = 65,
        Pinned = 69,
        Enum = 85,
        Any = 127,
    };
}
namespace app::ropeway::OptionManager {
    enum class ReticleColorType : int64_t {
        RED = 0,
        BLUE = 1,
        GREEN = 2,
        WHITE = 3,
    };
}
namespace app::ropeway::network::service::AchievementServiceController {
    enum class ProcessType : int64_t {
        Idle = 0,
        Setup = 1,
        Normal = 2,
        Failed = 3,
    };
}
namespace via::wwise {
    enum class FadeType : int64_t {
        None = 0,
        IN_ = 1,
        Out = 2,
    };
}
namespace app::ropeway::OptionManager {
    enum class VolumeLightQuality : int64_t {
        OFF = 0,
        LOW = 1,
        HIGH = 2,
        HIGHEST = 3,
    };
}
namespace via::render::Primitive {
    enum class ShapeType : int64_t {
        Sphere = 0,
        Tetrahedron = 1,
        Cube = 2,
        Cone = 3,
        Cylinder = 4,
        Max = 5,
    };
}
namespace app::ropeway::gamemastering::AddItemData::ItemData {
    enum class Category : int64_t {
        Ex = 0,
        Omake = 1,
    };
}
namespace app::ropeway::implement::userdata::ArmUserData::PartsSetting::AbilitySetting {
    enum class Calculation : int64_t {
        Invalid = 0xFFFFFFFF,
        Multi = 0,
        Add = 1,
    };
}
namespace app::ropeway::ReNet {
    enum class PostType : int64_t {
        Result = 0,
        Extra = 1,
        Hoken = 2,
    };
}
namespace app::ropeway::enemy::em5000::RoleMotionID {
    enum class DeadIdle : int64_t {
        DEAD_IDLE_00 = 4204,
    };
}
namespace app::ropeway::weapon::controller::SparkShellGeneratorControlUserData::GenerateTask {
    enum class StateKind : int64_t {
        Invalid = 0,
        WaitTime = 1,
        Generate = 2,
        CoolTime = 3,
        End = 4,
    };
}
namespace via::motion::IkDamageAction {
    enum class CalculationAddRotation : int64_t {
        SpecificJoint = 0,
        AllJoint = 1,
    };
}
namespace via::hid::camera::detail::PlayStationCameraLedTracker {
    enum class UpdateTaskStatus : int64_t {
        Wait = 0,
        Standup = 1,
        Run = 2,
        Shitdown = 3,
        Reboot = 4,
    };
}
namespace app::ropeway::Em6000Think {
    enum class FLAG : int64_t {
        AttackHited = 0,
        DamageHited = 1,
        ThreatEnable = 2,
        TargetOnStep = 3,
        DemarcationOver = 4,
        SwimRequest = 5,
        EyeBroken = 6,
    };
}
namespace via::render {
    enum class SamplerType : int64_t {
        PointWrap = 0,
        PointClamp = 1,
        PointBorder = 2,
        PointMirror = 3,
        BilinearWrap = 4,
        BilinearClamp = 5,
        BilinearBorder = 6,
        BilinearMirror = 7,
        TrilinearWrap = 8,
        TrilinearClamp = 9,
        TrilinearBorder = 10,
        TrilinearMirror = 11,
        Anisotropic2Wrap = 12,
        Anisotropic2Clamp = 13,
        Anisotropic2Border = 14,
        Anisotropic2Mirror = 15,
        Anisotropic4Wrap = 16,
        Anisotropic4Clamp = 17,
        Anisotropic4Border = 18,
        Anisotropic4Mirror = 19,
        Anisotropic8Wrap = 20,
        Anisotropic8Clamp = 21,
        Anisotropic8Border = 22,
        Anisotropic8Mirror = 23,
        Anisotropic16Wrap = 24,
        Anisotropic16Clamp = 25,
        Anisotropic16Border = 26,
        Anisotropic16Mirror = 27,
        PointCompare = 28,
        LinearCompare = 29,
        Max = 30,
        AutomaticWrap = 31,
        AutomaticClamp = 32,
        AutomaticBorder = 33,
        AutomaticMirror = 34,
    };
}
namespace via::navigation::AIMapEffector {
    enum class EdgeCreateType : int64_t {
        Optimize = 0,
        Simple = 1,
        NonConvexCheck = 2,
        OptimizeLegacy = 3,
    };
}
namespace via::motion::tree::CalculateNode {
    enum class Operation : int64_t {
        Add = 0,
        Sub = 1,
        Mul = 2,
        Div = 3,
        Mod = 4,
        Pow = 5,
        MIN_ = 6,
        Max = 7,
    };
}
namespace app::ropeway::gimmick::action::TriggerKeyCustomizer {
    enum class ButtonType : int64_t {
        LSTICK = 0,
    };
}
namespace app::ropeway::rogue::RogueEnemyKillCountUserData {
    enum class FlagSetTiming : int64_t {
        OnDead = 0,
        OnDestroy = 1,
    };
}
namespace app::ropeway::enemy::em3000 {
    enum class FLAG : int64_t {
        AttackHited = 0,
        AttackEnable = 1,
        ActiveAction = 2,
        FirstSoundDetection = 3,
        SoundDetection = 4,
        RequestFindThreat = 5,
        ChaseMode = 6,
        ForceWalkable = 7,
    };
}
namespace via::clr {
    enum class SystemType : int64_t {
        None = 0,
        Object = 1,
        String = 2,
        Array = 3,
        Delegate = 4,
        MulticastDelegate = 5,
        Enum = 6,
        ValueType = 7,
        Exception = 8,
        Attribute = 9,
        Thread = 10,
        ThreadStart = 11,
        Type = 12,
        Byte = 13,
        SByte = 14,
        Char = 15,
        Int16 = 16,
        UInt16 = 17,
        Int32 = 18,
        UInt32 = 19,
        Int64 = 20,
        UInt64 = 21,
        Single = 22,
        Double = 23,
        IntPtr = 24,
        UIntPtr = 25,
        Boolean = 26,
        DateTime = 27,
        TimeSpan = 28,
        Guid = 29,
        TypedReference = 30,
        Void = 31,
        RuntimeType = 32,
        RuntimeAssembly = 33,
        RuntimeConstructorInfo = 34,
        RuntimeEventInfo = 35,
        RuntimeFieldInfo = 36,
        RuntimeMethodInfo = 37,
        RuntimeModule = 38,
        RuntimeParameterInfo = 39,
        RuntimePropertyInfo = 40,
        RuntimeTypeHandle = 41,
        RuntimeMethodHandle = 42,
        RuntimeFieldHandle = 43,
        Assembly = 44,
        MemberInfo = 45,
        ConstructorInfo = 46,
        EventInfo = 47,
        FieldInfo = 48,
        MethodInfo = 49,
        Module = 50,
        ParameterInfo = 51,
        PropertyInfo = 52,
        FlagsAttribute = 53,
        InvalidCastException = 54,
        IndexOutOfRangeException = 55,
        NullReferenceException = 56,
        DivideByZeroException = 57,
        OverflowException = 58,
        ArgumentOutOfRangeException = 59,
        ArgumentNullException = 60,
        ArgumentException = 61,
        ArithmeticException = 62,
        OutOfMemoryException = 63,
        FormatException = 64,
        RankException = 65,
        ArrayTypeMismatchException = 66,
        NotImplementedException = 67,
        NotSupportedException = 68,
        ObjectDisposedException = 69,
        InvalidOperationException = 70,
        NotFiniteNumberException = 71,
        StackOverflowException = 72,
        ThreadAbortException = 73,
        ThreadStartException = 74,
        ThreadStateException = 75,
        TypeAccessException = 76,
        TargetException = 77,
        TargetInvocationException = 78,
        InternalEnumerator = 79,
        IEnumerableT = 80,
        ICollectionT = 81,
        IListT = 82,
        IComparable = 83,
        IComparer = 84,
        AppDomaIN_ = 85,
        IComparerT = 86,
        IEnumerator = 87,
        IComparableT = 88,
        Max = 89,
    };
}
namespace app::ropeway::InputDefine {
    enum class Command : int64_t {
        NONE = 0,
        QUICK_TURN = 1,
        USE_SUPPORT = 2,
    };
}
namespace via::navigation::map {
    enum class MapType : int64_t {
        NavMesh = 0,
        WayPoint = 1,
        VolumeSpace = 2,
        NoMap = 3,
    };
}
namespace app::ropeway::OptionManager {
    enum class DisplayModeCol : int64_t {
        Resolution = 0,
        Refreshrate = 1,
        DispKind = 2,
    };
}
namespace via::render::SSRControl {
    enum class SSRControlType : int64_t {
        Default = 0,
        Disable = 1,
        Spherical = 2,
        HighQuality = 3,
    };
}
namespace via::motion::JointExMultiRemapValue::OutputData {
    enum class Axis : int64_t {
        X = 0,
        Y = 1,
        Z = 2,
    };
}
namespace via::effect::script::EnvironmentEffectManager::ExternalProperty {
    enum class PlayState : int64_t {
        Idle = 0,
        Play = 1,
        Finish = 2,
        Kill = 3,
    };
}
namespace via::wwise {
    enum class ListenerBitMask : int64_t {
        ListenerBitMask_0 = 1,
        ListenerBitMask_1 = 2,
        ListenerBitMask_2 = 4,
        ListenerBitMask_3 = 8,
        ListenerBitMask_4 = 16,
        ListenerBitMask_5 = 32,
        ListenerBitMask_6 = 64,
        ListenerBitMask_7 = 128,
    };
}
namespace app::ropeway::gui::NewInventoryDetailBehavior {
    enum class GuideType : int64_t {
        NoDisp = 0,
        Detail = 1,
        Select = 2,
        Select2 = 3,
        PortableSafe = 4,
        Oscilloscope = 5,
        Next = 6,
    };
}
namespace app::ropeway::OptionManager {
    enum class Frame_Rate : int64_t {
        VARIABLE = 0,
        FPS60 = 1,
        FPS30 = 2,
    };
}
namespace via::render::command::PS4Extention {
    enum class ExtentionType : int64_t {
        ExtentionType_4KCheckerBoardRenderingEnable = 0,
        ExtentionType_4KCheckerBoardRenderingEvenFrameEnable = 0,
        ExtentionType_4KCheckerBoardRenderingOddFrameEnable = 1,
        ExtentionType_TemporalAntialiasing4KCheckerBoardRenderingEvenFrameEnable = 2,
        ExtentionType_TemporalAntialiasing4KCheckerBoardRenderingOddFrameEnable = 3,
        ExtentionType_4KCheckerBoardRenderingDisable = 4,
        ExtentionType_AlphaUnroll_Enable = 5,
        ExtentionType_AlphaUnroll_Disable = 6,
        ExtentionType_PsInvoke_Enable = 7,
        ExtentionType_PsInvoke_Disable = 8,
        ExtentionType_ObjectID_Enable = 9,
        ExtentionType_ObjectID_Disable = 10,
        ExtentionType_ForcedResolve = 11,
        ExtentionType_Resummarized = 12,
        ExtentionType_ForceEQAAx4DepthTestEnable = 13,
        ExtentionType_ForceEQAAx4Disable = 14,
    };
}
namespace app::ropeway::gui::NewInventoryBehavior {
    enum class DetailInitialMode : int64_t {
        Enter = 0,
        Normal = 1,
        Small = 2,
    };
}
namespace via::gui::GlowManager {
    enum class BlurFilterLayerType : int64_t {
        None = 0,
        Transparent = 1,
        PostEffect = 2,
        Overlay = 3,
    };
}
namespace app::ropeway::OptionManager {
    enum class CameraSpeedType : int64_t {
        MAXIMUM = 0,
        QUICK = 1,
        NORMAL = 2,
        SLOW = 3,
    };
}
namespace app::ropeway::supportitem::SupportItemAttachmentController {
    enum class ActionType : int64_t {
        AttackOn = 0,
        ItemSet = 1,
        ItemSetCansel = 2,
        DispOff = 3,
        DispOn = 4,
    };
}
namespace app::ropeway::fsmv2 {
    enum class CheckType : int64_t {
        EQUALS = 0,
        NOT_EQUALS = 1,
    };
}
namespace via {
    enum class SystemServiceAppParamIndex : int64_t {
        Index0 = 0,
        Index1 = 1,
        Index2 = 2,
        Index3 = 3,
        Index4 = 4,
        Index5 = 5,
        Index6 = 6,
        Index7 = 7,
        Max = 8,
    };
}
namespace app::ropeway::gimmick::action::GimmickPressed {
    enum class ActionType : int64_t {
        HOLD = 0,
        RELEASE = 1,
        NONE = 2,
    };
}
namespace app::ropeway::enemy::em7000::Em7000Define {
    enum class RequestSetID : int64_t {
        PRESS = 0,
        SENSOR_TOUCH = 1,
        SENSOR_HEARING = 2,
        ATTACK_BITE = 3,
    };
}
namespace app::ropeway::OptionManager {
    enum class ShadowMeshQuality : int64_t {
        LOW = 0,
        STANDARD = 1,
        HIGH = 2,
        HIGHEST = 3,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class WwiseEnemyBool : int64_t {
        FALSE_ = 0,
        TRUE_ = 1,
    };
}
namespace app::ropeway::navigation::Definition {
    enum class AttributeKind : int64_t {
        Attribute0 = 0,
        Attribute1 = 1,
        Invalid = 2,
    };
}
namespace app::ropeway::environment::lifecycle::BorderTerrainManager {
    enum class LoadStatus : int64_t {
        Unloaded = 0,
        Standby = 1,
        Active = 2,
    };
}
namespace app::ropeway::gamemastering::Map {
    enum class Area : int64_t {
        _0 = 0,
        _1 = 1,
        _2 = 2,
        _3 = 3,
        _4 = 4,
        _5 = 5,
        AREA_NUM = 6,
    };
}
namespace via::render::PrimitiveGUIResource {
    enum class ShaderType : int64_t {
        GUIPoly2D = 0,
        GUIPoly2DMask = 1,
        GUIPoly2DMaskReverse = 2,
        GUIPoly2DAdd = 3,
        GUIPoly2DAddMask = 4,
        GUIPoly2DAddMaskReverse = 5,
        GUIPoly2DDetone = 6,
        GUIPoly2DMaskDetone = 7,
        GUIPoly2DMaskReverseDetone = 8,
        GUIPoly2DAddDetone = 9,
        GUIPoly2DAddMaskDetone = 10,
        GUIPoly2DAddMaskReverseDetone = 11,
        GUIGlyph2D = 12,
        GUIGlyph2DMask = 13,
        GUIGlyph2DMaskReverse = 14,
        GUIGlyph2DAdd = 15,
        GUIGlyph2DAddMask = 16,
        GUIGlyph2DAddMaskReverse = 17,
        GUIGlyph2DDetone = 18,
        GUIGlyph2DMaskDetone = 19,
        GUIGlyph2DMaskReverseDetone = 20,
        GUIGlyph2DAddDetone = 21,
        GUIGlyph2DAddMaskDetone = 22,
        GUIGlyph2DAddMaskReverseDetone = 23,
        GUIPoly3D = 24,
        GUIPoly3DTest = 25,
        GUIPoly3DMask = 26,
        GUIPoly3DMaskTest = 27,
        GUIPoly3DMaskReverse = 28,
        GUIPoly3DMaskReverseTest = 29,
        GUIPoly3DAdd = 30,
        GUIPoly3DAddTest = 31,
        GUIPoly3DAddMask = 32,
        GUIPoly3DAddMaskTest = 33,
        GUIPoly3DAddMaskReverse = 34,
        GUIPoly3DAddMaskReverseTest = 35,
        GUIPoly3DDetone = 36,
        GUIPoly3DTestDetone = 37,
        GUIPoly3DMaskDetone = 38,
        GUIPoly3DMaskTestDetone = 39,
        GUIPoly3DMaskReverseDetone = 40,
        GUIPoly3DMaskReverseTestDetone = 41,
        GUIPoly3DAddDetone = 42,
        GUIPoly3DAddTestDetone = 43,
        GUIPoly3DAddMaskDetone = 44,
        GUIPoly3DAddMaskTestDetone = 45,
        GUIPoly3DAddMaskReverseDetone = 46,
        GUIPoly3DAddMaskReverseTestDetone = 47,
        GUIGlyph3D = 48,
        GUIGlyph3DTest = 49,
        GUIGlyph3DMask = 50,
        GUIGlyph3DMaskTest = 51,
        GUIGlyph3DMaskReverse = 52,
        GUIGlyph3DMaskReverseTest = 53,
        GUIGlyph3DAdd = 54,
        GUIGlyph3DAddTest = 55,
        GUIGlyph3DAddMask = 56,
        GUIGlyph3DAddMaskTest = 57,
        GUIGlyph3DAddMaskReverse = 58,
        GUIGlyph3DAddMaskReverseTest = 59,
        GUIGlyph3DDetone = 60,
        GUIGlyph3DTestDetone = 61,
        GUIGlyph3DMaskDetone = 62,
        GUIGlyph3DMaskTestDetone = 63,
        GUIGlyph3DMaskReverseDetone = 64,
        GUIGlyph3DMaskReverseTestDetone = 65,
        GUIGlyph3DAddDetone = 66,
        GUIGlyph3DAddTestDetone = 67,
        GUIGlyph3DAddMaskDetone = 68,
        GUIGlyph3DAddMaskTestDetone = 69,
        GUIGlyph3DAddMaskReverseDetone = 70,
        GUIGlyph3DAddMaskReverseTestDetone = 71,
        GUIPolyPers = 72,
        GUIPolyPersTest = 73,
        GUIPolyPersMask = 74,
        GUIPolyPersMaskTest = 75,
        GUIPolyPersMaskReverse = 76,
        GUIPolyPersMaskReverseTest = 77,
        GUIPolyPersAdd = 78,
        GUIPolyPersAddTest = 79,
        GUIPolyPersAddMask = 80,
        GUIPolyPersAddMaskTest = 81,
        GUIPolyPersAddMaskReverse = 82,
        GUIPolyPersAddMaskReverseTest = 83,
        GUIGlyphPers = 84,
        GUIGlyphPersTest = 85,
        GUIGlyphPersMask = 86,
        GUIGlyphPersMaskTest = 87,
        GUIGlyphPersMaskReverse = 88,
        GUIGlyphPersMaskReverseTest = 89,
        GUIGlyphPersAdd = 90,
        GUIGlyphPersAddTest = 91,
        GUIGlyphPersAddMask = 92,
        GUIGlyphPersAddMaskTest = 93,
        GUIGlyphPersAddMaskReverse = 94,
        GUIGlyphPersAddMaskReverseTest = 95,
        GUIPoly2DMaskWrite = 96,
        GUIPoly3DMaskWrite = 97,
        GUIPoly3DMaskWriteTest = 98,
        GUIPolyPersMaskWrite = 99,
        GUIPolyPersMaskWriteTest = 100,
        GUIGlyph2DMaskWrite = 101,
        GUIGlyph3DMaskWrite = 102,
        GUIGlyph3DMaskWriteTest = 103,
        GUIGlyphPersMaskWrite = 104,
        GUIGlyphPersMaskWriteTest = 105,
        GUIPoly2DMaskWriteBlend2 = 106,
        GUIPoly3DMaskWriteBlend2 = 107,
        GUIPoly3DMaskWriteTestBlend2 = 108,
        GUIPolyPersMaskWriteBlend2 = 109,
        GUIPolyPersMaskWriteTestBlend2 = 110,
        GUIGlyph2DMaskWriteBlend2 = 111,
        GUIGlyph3DMaskWriteBlend2 = 112,
        GUIGlyph3DMaskWriteTestBlend2 = 113,
        GUIGlyphPersMaskWriteBlend2 = 114,
        GUIGlyphPersMaskWriteTestBlend2 = 115,
        GUIPoly2DMaskWriteBlend3 = 116,
        GUIPoly3DMaskWriteBlend3 = 117,
        GUIPoly3DMaskWriteTestBlend3 = 118,
        GUIPolyPersMaskWriteBlend3 = 119,
        GUIPolyPersMaskWriteTestBlend3 = 120,
        GUIGlyph2DMaskWriteBlend3 = 121,
        GUIGlyph3DMaskWriteBlend3 = 122,
        GUIGlyph3DMaskWriteTestBlend3 = 123,
        GUIGlyphPersMaskWriteBlend3 = 124,
        GUIGlyphPersMaskWriteTestBlend3 = 125,
        GUIGlyphGlow2D = 126,
        GUIGlyphGlow2DMask = 127,
        GUIGlyphGlow2DMaskReverse = 128,
        GUIGlyphGlow2DAdd = 129,
        GUIGlyphGlow2DAddMask = 130,
        GUIGlyphGlow2DAddMaskReverse = 131,
        GUIGlyphGlow2DDetone = 132,
        GUIGlyphGlow2DMaskDetone = 133,
        GUIGlyphGlow2DMaskReverseDetone = 134,
        GUIGlyphGlow2DAddDetone = 135,
        GUIGlyphGlow2DAddMaskDetone = 136,
        GUIGlyphGlow2DAddMaskReverseDetone = 137,
        GUIGlyphGlow3D = 138,
        GUIGlyphGlow3DTest = 139,
        GUIGlyphGlow3DMask = 140,
        GUIGlyphGlow3DMaskTest = 141,
        GUIGlyphGlow3DMaskReverse = 142,
        GUIGlyphGlow3DMaskReverseTest = 143,
        GUIGlyphGlow3DAdd = 144,
        GUIGlyphGlow3DAddTest = 145,
        GUIGlyphGlow3DAddMask = 146,
        GUIGlyphGlow3DAddMaskTest = 147,
        GUIGlyphGlow3DAddMaskReverse = 148,
        GUIGlyphGlow3DAddMaskReverseTest = 149,
        GUIGlyphGlow3DDetone = 150,
        GUIGlyphGlow3DTestDetone = 151,
        GUIGlyphGlow3DMaskDetone = 152,
        GUIGlyphGlow3DMaskTestDetone = 153,
        GUIGlyphGlow3DMaskReverseDetone = 154,
        GUIGlyphGlow3DMaskReverseTestDetone = 155,
        GUIGlyphGlow3DAddDetone = 156,
        GUIGlyphGlow3DAddTestDetone = 157,
        GUIGlyphGlow3DAddMaskDetone = 158,
        GUIGlyphGlow3DAddMaskTestDetone = 159,
        GUIGlyphGlow3DAddMaskReverseDetone = 160,
        GUIGlyphGlow3DAddMaskReverseTestDetone = 161,
        GUIGlyphGlowPers = 162,
        GUIGlyphGlowPersTest = 163,
        GUIGlyphGlowPersMask = 164,
        GUIGlyphGlowPersMaskTest = 165,
        GUIGlyphGlowPersMaskReverse = 166,
        GUIGlyphGlowPersMaskReverseTest = 167,
        GUIGlyphGlowPersAdd = 168,
        GUIGlyphGlowPersAddTest = 169,
        GUIGlyphGlowPersAddMask = 170,
        GUIGlyphGlowPersAddMaskTest = 171,
        GUIGlyphGlowPersAddMaskReverse = 172,
        GUIGlyphGlowPersAddMaskReverseTest = 173,
        Max = 174,
    };
}
namespace app::ropeway::posteffect::RenderOutputController {
    enum class ImageQualityLimit : int64_t {
        NoLimited = 0,
        Limited = 1,
        ReleaseCheck = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickDialLockBehavior {
    enum class Dial : int64_t {
        DIAL_1 = 0,
        DIAL_2 = 1,
        DIAL_3 = 2,
        MAX = 3,
    };
}
namespace app::ropeway::enemy::tracks::Em7300ColliderTrack {
    enum class RequestSetID : int64_t {
        Invalid = 0xFFFFFFFF,
        Press = 0,
        Damage = 1,
        Sensor_Touch = 2,
        Attack_SecondArmL = 3,
        Attack_SecondArmR = 4,
        DamageWeak = 5,
        Attack_Hold = 6,
        AttackBurst_L = 7,
        AttackBurst_R = 8,
        AttackToss_L = 9,
        AttackToss_R = 10,
        DrawControl = 11,
        Decal_AttackBurst_L = 12,
        Decal_AttackBurst_R = 13,
        Decal_AttackToss_L = 14,
        Decal_AttackToss_R = 15,
        Sensor_Gimmick = 16,
        WeakCover00 = 17,
        WeakCover01 = 18,
        WeakCover02 = 19,
        WeakCover03 = 20,
        WeakCover04 = 21,
        WeakCover05 = 22,
        WeakCover06 = 23,
        WeakCover07 = 24,
        DamageWeak00 = 25,
        DamageWeak01 = 26,
        DamageWeak02 = 27,
        DamageWeak03 = 28,
        DamageWeak04 = 29,
        DamageWeak05 = 30,
        DamageWeak06 = 31,
        DamageWeak07 = 32,
        DamageWeak08 = 33,
        DamageWeak09 = 34,
        DamageWeak10 = 35,
        Attack_Dash_Start = 36,
        Attack_Train_Landing = 37,
        Attack_Wall_Landing_Outside = 38,
        Attack_Wall_Landing_Center = 39,
        Attack_Train_Landing_Outside = 40,
        AttackDeadDown_L = 41,
        AttackDeadDown_R = 42,
        AimTarget = 43,
    };
}
namespace app::ropeway::survivor::npc::NpcOrderDefine {
    enum class CommonCommandKind : int64_t {
        SetActive = 0,
        SetCondition = 1,
        SetNarrowArea = 2,
        SetPressPriority = 3,
        EnableFaceLight = 4,
        DisableFaceLight = 5,
        EnableNoticePoint = 6,
        Invalid = 7,
    };
}
namespace app::ropeway::enemy::em5000::MotionPattern {
    enum class Hold : int64_t {
        Front = 0,
        Left = 1,
        Right = 2,
        Back_LeftTurn = 3,
        Back_RightTurn = 4,
    };
}
namespace app::ropeway::SoundAppealUserData {
    enum class AppealPriority : int64_t {
        Priority_00 = 0,
        Priority_01 = 1,
        Priority_02 = 2,
        Priority_03 = 3,
        Priority_04 = 4,
        Priority_05 = 5,
        Priority_06 = 6,
        Priority_07 = 7,
        Max = 8,
    };
}
namespace app::ropeway::StaffRollSingleData {
    enum class SKUType : int64_t {
        All = 0,
        WW = 1,
        JP = 2,
    };
}
namespace app::ropeway::fsmv2::GmkSewageDrawBridge {
    enum class State : int64_t {
        SET = 0,
        RESTORE = 1,
    };
}
namespace via::os {
    enum class SocketType : int64_t {
        None = 0,
        Tcp = 1,
        Udp = 2,
        Ssl = 3,
    };
}
namespace via::hid::camera {
    enum class PlayStationCameraFormat : int64_t {
        Unknown = 0,
        NoUse = 1,
        BaseYUV422 = 65536,
        BaseRAW16 = 65537,
        BaseRAW8 = 65538,
        ScaleYUV422 = 131072,
        ScaleY16 = 131073,
        ScaleY8 = 131074,
    };
}
namespace app::ropeway::gamemastering::RogueUIMapManager {
    enum class State : int64_t {
        INVALID = 0xFFFFFFFF,
        INITIALIZE = 0,
        WAIT_TO_INGAME = 1,
        INITIALIZE_TO_INGAME = 2,
        INGAME = 3,
        ERROR_ = 4,
    };
}
namespace via::hid::virtualKeyboard::nsw {
    enum class KeyboardMode : int64_t {
        Full = 0,
        Numeric = 1,
        ASCII = 2,
        FullLatIN_ = 3,
    };
}
namespace via::render {
    enum class DsvDimension : int64_t {
        Unknown = 0,
        Texture1d = 1,
        Texture1darray = 2,
        Texture2d = 3,
        Texture2darray = 4,
        Texture2dms = 5,
        Texture2dmsarray = 6,
    };
}
namespace via::behaviortree::action::NodeTimer {
    enum class Unit : int64_t {
        Second = 0,
        Frame = 1,
    };
}
namespace app::ropeway::enemy::em3000::Sick {
    enum class Type : int64_t {
        FLASH = 0,
        FIRE = 1,
        ACID = 2,
    };
}
namespace app::ropeway::SurvivorDefine::Damage {
    enum class Power : int64_t {
        Unspecified = 0,
        Small = 65536,
        Medium = 131072,
        Large = 196608,
        ExtraLarge = 262144,
    };
}
namespace via::hid::HDVibrationSource {
    enum class TargetNode : int64_t {
        Dual = 0,
        Left = 1,
        Right = 2,
    };
}
namespace app::ropeway::RogueActivityLogManager {
    enum class ActivityLogMode : int64_t {
        Invalid = 0xFFFFFFFF,
        AIM_FADEIN_ = 0,
        AIM_FADEOUT = 1,
    };
}
namespace app::ropeway::enemy::em3000::Em3000SearchingController {
    enum class State : int64_t {
        CAUTION = 0,
        SERCHING = 1,
        NONE = 2,
    };
}
namespace app::ropeway::WwisePropertyIngameBehavior {
    enum class TitleBgType : int64_t {
        LOADING_FF = 0,
        LOADING_00 = 1,
        LOADING_01 = 2,
        LOADING_02 = 3,
        LOADING_03 = 4,
        LOADING_04 = 5,
        LOADING_05 = 6,
        LOADING_06 = 7,
        LOADING_07 = 8,
        LOADING_08 = 9,
        LOADING_09 = 10,
        LOADING_10 = 11,
        LOADING_11 = 12,
        LOADING_12 = 13,
        LOADING_99 = 14,
        EXTRA_0 = 15,
        EXTRA_1 = 16,
        EXTRA_2 = 17,
        RECORD = 18,
        CONCEPTART = 19,
        FIGURE = 20,
        COSTUME = 21,
        LOST_0 = 22,
        LOST_1 = 23,
        LOST_2 = 24,
        LOST_3 = 25,
    };
}
namespace app::ropeway::gimmick::action::GimmickCircuitPuzzle {
    enum class PanelType : int64_t {
        NONE = 0,
        STRAIGHT = 1,
        CURVE_SINGLE = 2,
        CURVE_DOUBLE = 3,
        CROSS = 4,
        T_INTERSECTION = 5,
        GENERATOR = 6,
    };
}
namespace app::ropeway::survivor::npc::NpcParam {
    enum class SpeedType : int64_t {
        RELATIVE_JOG_START = 0,
        ANY = 1,
    };
}
namespace via::navigation::Navigation {
    enum class TraceLineOptimizeTiming : int64_t {
        NodeUpdate = 0,
        Frame = 1,
        Start = 2,
    };
}
namespace via::gui {
    enum class AssetLanguage : int64_t {
        Invalid = 0xFFFFFFFF,
        No0 = 0,
        No1 = 1,
        No2 = 2,
        No3 = 3,
        Max = 4,
    };
}
namespace app::ropeway::level::LoadGimmickSpecCatalog::Order {
    enum class OrderMode : int64_t {
        Simple = 0,
        IfOne = 1,
        ExclusiveSuffix = 2,
    };
}
namespace app::ropeway::MansionManager::NeighboringAreaInfo::NeighboringInfo {
    enum class Attribute : int64_t {
        None = 0,
        DisconnectOnDynamicSpace = 1,
    };
}
namespace app::ropeway::gui::RogueInventoryBehavior {
    enum class State : int64_t {
        NoMove = 0,
        Enter = 1,
        Capture = 2,
        Ready = 3,
        Ready2 = 4,
        Move = 5,
        Leave = 6,
        Finish = 7,
    };
}
namespace via::effect::script::EffectDecal {
    enum class DecalDirectionEnum : int64_t {
        XPlus = 0,
        XMinus = 1,
        YPlus = 2,
        YMinus = 3,
        ZPlus = 4,
        ZMinus = 5,
    };
}
namespace app::ropeway::gimmick::action::TriggerSight {
    enum class SightMode : int64_t {
        LIGHT = 0,
        SCREEN = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickRogueBaseArea {
    enum class ArgNo : int64_t {
        Open = 0,
        Wait = 1,
    };
}
namespace via::gui {
    enum class UserParamType : int64_t {
        Int = 0,
        Float = 1,
        String = 2,
    };
}
namespace via::gui::GlowManager {
    enum class BlurFilterMipMap : int64_t {
        MipMapMax = 6,
    };
}
namespace app::ropeway::gimmick::option::ObjectStateSettings::ObjectStateInfo {
    enum class LayerType : int64_t {
        ALL = 1,
        MARKER = 2,
        TERRAIN_ = 4,
    };
}
namespace via::hid {
    enum class VibrationOutputMotor : int64_t {
        Motor0 = 0,
        Motor1 = 1,
        Motor2 = 2,
        Motor3 = 3,
        All = 128,
        Invalid = 0xFFFFFFFF,
    };
}
namespace app::ropeway::fsmv2::FlagControlActionRoot {
    enum class ActionTiming : int64_t {
        Start = 0,
        End = 1,
    };
}
namespace app::ropeway::gimmick::action::AreaAccessSensor {
    enum class TargetType : int64_t {
        PL_POS_AND_CAM_ANGLE = 0,
        TARGET_OBJECT = 1,
        EM_FROM_CONTEXT = 2,
        PL = 3,
        TARGET_NPC = 4,
    };
}
namespace app::ropeway::Enemy3DNaviMoveSupporter {
    enum class CheckResultType : int64_t {
        SUCCESS = 0,
        FAILED_ANGLE_LIMIT = 1,
        FAILED_DISTANCE_LIMIT = 2,
    };
}
namespace app::ropeway::enemy::em7400::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_MOVE = 1,
        ACT_FINAL = 2,
        ATK_GRAPPLE = 2000,
        DIE_NORMAL = 5000,
        DIE_RIGID = 5001,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace app::ropeway::TwoBoneIK {
    enum class CalculateMode : int64_t {
        HingeIK = 0,
        HingeIKArc = 1,
    };
}
namespace via::hid {
    enum class GestureFingerNumber : int64_t {
        One = 0,
        Two = 1,
        Three = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0200_MONTAGE_PARTS_BODY : int64_t {
        BODY00 = 0,
        BODY02 = 1,
        BODY70 = 2,
        NONE = 3,
    };
}
namespace via::render::MaterialParam {
    enum class MaterialParamType : int64_t {
        Unkown = 0,
        Float4 = 1,
        Float = 2,
        Texture = 3,
    };
}
namespace app::ropeway::WwiseParamCurveTrigger {
    enum class TriggerType : int64_t {
        Value = 0,
        Velocity = 1,
    };
}
namespace via::navigation::NodeQueryInfo {
    enum class RegionType : int64_t {
        None = 0,
        Sphere = 1,
        AABB = 2,
        OBB = 3,
        Capsule = 4,
        Cylinder = 5,
        LineSegment = 6,
        Collidable = 7,
    };
}
namespace via::render {
    enum class ShaderStage : int64_t {
        Vertex = 0,
        Hull = 1,
        DomaIN_ = 2,
        Geometry = 3,
        Pixel = 4,
        Compute = 5,
        Max = 6,
    };
}
namespace via::movie {
    enum class DecodeLayer : int64_t {
        Common = 0,
        Overlay = 1,
    };
}
namespace app::ropeway::gui::RogueItemBoxBehavior {
    enum class Mode : int64_t {
        Disable = 0,
        DisableBlackOut = 1,
        EnableDisplay = 2,
        EnableDisplayNoMove = 3,
        EnableDisplayAndControl = 4,
        EnableDisplayAndControlFree = 5,
    };
}
namespace app::ropeway::ObjectPoolManager {
    enum class Phase : int64_t {
        Initial = 0,
        Working = 1,
    };
}
namespace via::areamap::test::DotProductSelf {
    enum class Axis : int64_t {
        X = 0,
        Y = 1,
        Z = 2,
    };
}
namespace app::ropeway {
    enum class AccountPickerResult : int64_t {
        ActiveUserFirstChanged = 0,
        ActiveUserChanged = 1,
        ActiveUserUnchanged = 2,
        InvalidUser = 3,
        GuestUser = 4,
        DeviceDisconnected = 5,
        Failed = 6,
    };
}
namespace app::ropeway::camera {
    enum class CameraMotionStatusType : int64_t {
        STOP = 0,
        PLAY = 1,
    };
}
namespace via::browser::utility::RequestManager {
    enum class Attr : int64_t {
        None = 0,
        NeedAnswer = 1,
        IsReentrant = 2,
        ManageLastArg = 4,
    };
}
namespace via::motion::IkTrain {
    enum class ControlType : int64_t {
        AutoPassTargetPosition = 0,
        HeadTargetPosition = 1,
    };
}
namespace app::ropeway::ReNetData::MainProgressData {
    enum class ProgressType : int64_t {
        PROGRESS_000 = 0,
        PROGRESS_001 = 1,
        PROGRESS_002 = 2,
        PROGRESS_003 = 3,
        PROGRESS_004 = 4,
        PROGRESS_005 = 5,
        PROGRESS_006 = 6,
        PROGRESS_007 = 7,
        PROGRESS_008 = 8,
        PROGRESS_009 = 9,
        PROGRESS_010 = 10,
        PROGRESS_011 = 11,
        PROGRESS_012 = 12,
        PROGRESS_013 = 13,
        PROGRESS_014 = 14,
        PROGRESS_015 = 15,
        PROGRESS_016 = 16,
        PROGRESS_017 = 17,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::Rotate {
    enum class ExecuteType : int64_t {
        Once = 0,
        Range = 1,
        Always = 2,
    };
}
namespace app::ropeway::gui::OptionResultTextureLoader {
    enum class ExecType : int64_t {
        RequestStandby = 0,
        GetTexture = 1,
    };
}
namespace app::ropeway::enemy::tracks::Em4100ColliderTrack {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Press = 0,
        Damage = 1,
        Sensor_Touch = 2,
        Attack = 3,
    };
}
namespace app::ropeway::behaviortree::condition::BtCondition_PL_ActionCheck {
    enum class CheckConditionType : int64_t {
        Idle = 0,
        Walk = 1,
        Jog = 2,
        Move = 3,
        Strafe = 4,
        GimmickBoxMove = 5,
        Cover = 6,
    };
}
namespace via::effect::script::CreatedEffectContainer::RequestCommand {
    enum class Command : int64_t {
        Invalid = 0xFFFFFFFF,
        None = 0,
        SetPlaySpeed = 1,
        SetExternParameter = 2,
        SetRootColor = 3,
    };
}
namespace app::ropeway::enemy::em3000::Em3000FootAdjust {
    enum class CastState : int64_t {
        TRUE_ = 0,
        FALSE_ = 1,
        WAIT = 2,
    };
}
namespace app::ropeway::enemy::em6100 {
    enum class RequestSetID : int64_t {
        INVALID = 0xFFFFFFFF,
        DAMAGE = 0,
        ATTACK = 1,
    };
}
namespace app::ropeway::JackDominator {
    enum class Priority : int64_t {
        HIGHEST = 0,
        EVENT = 1,
        GIMMICK = 2,
        ENEMY = 3,
        EM7000 = 4,
        EM7100 = 5,
        EM6200 = 6,
        EM5000 = 7,
        EM3000 = 8,
        EM0000 = 9,
        ENEMY_LOWEST = 10,
        GIMMICK_LOW = 11,
        LOWEST = 12,
        DEFAULT = 12,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class FakeDead : int64_t {
        DEAD_F_00 = 0,
        DEAD_F_01 = 1,
        DEAD_F_02 = 2,
        DEAD_B_00 = 3,
        DEAD_B_01 = 4,
        DEAD_B_02 = 5,
        DEAD_B_03 = 6,
        DEAD_LOW_F_00 = 7,
        DEAD_LOW_F_01 = 8,
        DEAD_LOW_F_02 = 9,
        DEAD_LOW_F_03 = 10,
        DEAD_LOW_B_00 = 11,
        DEAD_LOW_B_01 = 12,
        DEAD_LOW_B_02 = 13,
        DEAD_LOW_B_03 = 14,
        DEAD_FACEDOWN_00 = 15,
        DEAD_FACEDOWN_01 = 16,
        DEAD_FACEDOWN_02 = 17,
        DEAD_FACEUP_00 = 18,
        DEAD_FACEUP_01 = 19,
        DEAD_FACEUP_02 = 20,
        DEAD_FACEUP_03 = 21,
        DEAD_FACEUP_04 = 22,
        DEAD_INERTIA_00 = 23,
        DEAD_INERTIA_01 = 24,
        DEAD_INERTIA_02 = 25,
        DEAD_LOST_L_00 = 26,
        DEAD_LOST_L_01 = 27,
        DEAD_LOST_R_00 = 28,
        DEAD_LOST_R_01 = 29,
        DEAD_STOMPED_FACEDOWN_00 = 30,
        DEAD_STOMPED_FACEUP_00 = 31,
    };
}
namespace app::ropeway::LocationJumpController {
    enum class JumpPositionAttribute : int64_t {
        DEFAULT = 0,
        LEON_START_POS = 1,
        CLAIRE_START_POS = 2,
        LEON_B_START_POS = 3,
        CLAIRE_B_START_POS = 4,
        HUNK_START_POS = 5,
        TOFU_START_POS = 6,
        ROGUE_START_POS = 7,
        LEON_2ND_START_POS = 8,
        CLAIRE_2ND_START_POS = 9,
        MAX = 10,
        INVALID = 11,
    };
}
namespace via::render::VolumeDecal {
    enum class ProjectionShape : int64_t {
        Obb = 0,
        Pyramid = 2,
        PyramidY = 3,
        Max = 4,
    };
}
namespace app::ropeway::InputDefine {
    enum class InputMode : int64_t {
        Pad = 0,
        MouseKeyboard = 1,
    };
}
namespace via::behaviortree::TreeNodeData {
    enum class NodeAttribute : int64_t {
        IsEnabled = 1,
        IsRestartable = 2,
        HasReferenceTree = 4,
        BubblesChildEnd = 8,
        SelectOnce = 16,
        IsFSMNode = 32,
        TraverseToLeaf = 64,
    };
}
namespace via::gui {
    enum class TypingCondition : int64_t {
        Update = 0,
        Pause = 1,
        End = 2,
        Unknown = 3,
    };
}
namespace via::gui {
    enum class LetterAlignment : int64_t {
        LeftTop = 0,
        LeftCenter = 4,
        LeftBottom = 8,
        LeftBaseline = 12,
        CenterTop = 1,
        CenterCenter = 5,
        CenterBottom = 9,
        CenterBaseline = 13,
        RightTop = 2,
        RightCenter = 6,
        RightBottom = 10,
        RightBaseline = 14,
    };
}
namespace via::physics {
    enum class MaskType : int64_t {
        AND = 0,
        NAND = 1,
        Default = 2,
        Max = 3,
    };
}
namespace app::ropeway::EffectReceiverFromGimmick {
    enum class PlayType : int64_t {
        UNTIL_BOOT = 0,
        AFTER_BOOT = 1,
        ON_BOOT = 2,
    };
}
namespace app::ropeway::Em4100Define {
    enum class MoveTargetType : int64_t {
        NONE = 0,
        HOME_POSITION = 1,
    };
}
namespace via::render::ShadowQualityControl {
    enum class ShadowQualityType : int64_t {
        Default = 0,
        Fast = 1,
    };
}
namespace app::ropeway::gui::GimmickHotHouseCtrlPanelGuiBehavior {
    enum class IconState : int64_t {
        ERROR_ = 0,
        OPEN = 1,
        OPEN_01 = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class BreakableParts : int64_t {
        FACE = 0,
        CHEST = 1,
        WAIST = 2,
        L_SHOULDER = 3,
        L_UPPERARM = 4,
        L_FOREARM = 5,
        R_SHOULDER = 6,
        R_UPPERARM = 7,
        R_FOREARM = 8,
        L_THIGH = 9,
        L_KNEE = 10,
        L_SHIN_ = 11,
        L_ANKLE = 12,
        R_THIGH = 13,
        R_KNEE = 14,
        R_SHIN_ = 15,
        R_ANKLE = 16,
        HAT = 17,
    };
}
namespace app::ropeway::gui::GameSceneLoadingBehavior {
    enum class LoadingTrackId : int64_t {
        LOADING_00 = 0,
        LOADING_01 = 1,
        LOADING_02 = 2,
        LOADING_03 = 3,
        LOADING_04 = 4,
        LOADING_05 = 5,
        LOADING_06 = 6,
        LOADING_07 = 7,
        LOADING_08 = 8,
        LOADING_09 = 9,
        LOADING_10 = 10,
        LOADING_11 = 11,
        LOADING_12 = 12,
        LOADING_30 = 13,
        LOADING_40 = 14,
        LOADING_50 = 15,
        LOADING_51 = 16,
        LOADING_52 = 17,
        LOADING_53 = 18,
        LOADING_54 = 19,
        LOADING_60 = 20,
        LOADING_61 = 21,
        LOADING_62 = 22,
        LOADING_63 = 23,
        LOADING_99 = 24,
        MAX = 25,
        INVALID = 26,
    };
}
namespace app::ropeway::Em4000Define {
    enum class ActionStatus : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_MOVE = 2,
        ACT_JUMP = 3,
        ACT_FALL = 4,
        ACT_FALL_END = 5,
        ACT_FALL_END_RUN = 6,
        ACT_PIVOT_TURN = 7,
        ACT_STEP = 8,
        ACT_STEP_B = 9,
        ACT_REACTION_A = 10,
        ACT_REACTION_B = 11,
        ACT_CITY_REACTION = 12,
        ACT_DIG_LOOP = 13,
        ACT_DIG_END = 14,
        ACT_EAT_LOOP = 15,
        ACT_EAT_END = 16,
        ACT_LIEDOWN_LOOP = 17,
        ACT_LIEDOWN_END = 18,
        ACT_HOLE_WALK = 19,
        ACT_HOLE_WAIT = 20,
        ACT_HOLE_EXIT = 21,
        ACT_BARRICADE_LOOP = 22,
        ACT_GOTO_ROTATE = 23,
        ACT_FIND_REACTION = 24,
        ACT_FENCE_CLIMB = 25,
        ACT_KENNEL_BARKING = 26,
        ACT_KENNEL_IDLE = 27,
        ACT_CAR_CLIMB = 28,
        ATK_JUMP_BITE = 2000,
        ATK_GRAPPLE = 2001,
        ATK_GRAPPLE_BACK = 2002,
        ATK_FENCE_CLIMB = 2003,
        ATK_JUMP_DOOR = 2004,
        DMG_KNOCKBACK_BODY_F = 3000,
        DMG_KNOCKBACK_HEAD_F = 3001,
        DMG_KNOCKBACK_BODY_BL = 3002,
        DMG_KNOCKBACK_BODY_BR = 3003,
        DMG_STUN_HEAD_F = 3004,
        DMG_STUN_BODY_F = 3005,
        DMG_BURST_F = 3006,
        DMG_BURST_B = 3007,
        DMG_BURST_L = 3008,
        DMG_BURST_R = 3009,
        DMG_DOWN_FROM_RUN = 3010,
        DMG_STAGGER_R = 3011,
        DMG_STAGGER_L = 3012,
        DMG_ATK_BURST = 3013,
        DMG_BARRICADE = 3014,
        DMG_FLASH = 3015,
        DMG_ACID = 3016,
        DMG_KENNEL = 3017,
        DMG_RUN = 3018,
        DMG_ELECTRIC = 3019,
        DMG_ELECTRIC_FROM_DOWN = 3020,
        DMG_SPT_GRENADE = 4000,
        DMG_SPT_FLASHGRENADE = 4001,
        DMG_SPT_KNIFE = 4002,
        DIE_NORMAL = 5000,
        DIE_FROM_DOWN = 5001,
        DIE_FROM_RUN = 5002,
        DIE_FROM_JUMP = 5003,
        DIE_FROM_JUMP_NEAR = 5004,
        DIE_KENNEL = 5005,
        DIE_RIGID = 5006,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace via::network {
    enum class RequestId : int64_t {
        ContextStart = 257,
        P2pConnect = 513,
        SessionCreate = 769,
        SessionSearchByFilter = 770,
        SessionSearchByFriend = 771,
        SessionJoIN_ = 772,
        SessionKick = 774,
        SessionInvite = 775,
        SessionSetSearchKey = 776,
        SessionSetCustomProperty = 777,
        SessionSetPrivate = 778,
        SessionSetClose = 779,
        RankingRegister = 1025,
        RankingGetScoreByRange = 1026,
        RankingGetScoreByUniqueId = 1027,
        RankingGetScoreByFriend = 1028,
        AchievementsWrite = 1281,
        AchievementsRead = 1282,
        AchievementsUnlock = 1283,
        AchievementsGetInfo = 1284,
        StorageGetInfo = 1537,
        StorageUnlink = 1538,
        StorageOpen = 1539,
        StorageWrite = 1540,
        StorageRead = 1541,
        InvitationJoIN_ = 2049,
        IdConverterGetNickname = 2816,
        UtilityShowProfile = 3072,
        SessionLock = 773,
    };
}
namespace app::ropeway::PostEffectControllerUnitRoot {
    enum class Status : int64_t {
        Init = 0,
        EditMode = 1,
        PlayMode = 2,
    };
}
namespace app::ropeway::SurvivorAssignManager {
    enum class Phase : int64_t {
        WaitingRequest = 0,
        WaitingProcess = 1,
        WaitingActivePlayer = 2,
    };
}
namespace app::ropeway::ReNetData {
    enum class Type : int64_t {
        None = 0,
        Header = 1,
        Basic = 2,
        MainProgress = 4,
        ExtraProgress = 8,
        MainWeapon = 16,
        SubWeapon = 32,
        Bullet = 64,
        HealItem = 128,
        CombineItem = 256,
        DropItem = 512,
        DefeatEnemy = 1024,
        BreakParts = 2048,
        GameOver = 4096,
        Costume = 8192,
        BonusModePlay = 16384,
        DlcPlay = 32768,
        DlcInstall = 65536,
        PuzzlePlay = 131072,
        Other = 262144,
        GameClear = 524288,
        GameClearExtra = 1048576,
        WeeklyChallenge = 2097152,
        Record = 4194304,
        Env = 8388608,
        Option = 16777216,
        WeeklyChallengeExtra = 33554432,
        GameClearLost = 67108864,
        RecordLost = 134217728,
        WeaponFlags = 112,
        ItemFlags = 896,
        EnemyFlags = 3072,
    };
}
namespace app::ropeway::weapon::shell::ShellBase {
    enum class RadiateShellStatusInfomation : int64_t {
        NoSuspend = 0,
        Update = 1,
        LateUpdate = 2,
    };
}
namespace app::ropeway::RogueActivityLogManager {
    enum class ActivityLogType : int64_t {
        Invalid = 0xFFFFFFFF,
        Default = 0,
        InArea = 1,
        LevelUp = 2,
        InBossStage = 3,
        EnemyDead = 4,
        GetItem = 5,
        BuyItem = 6,
        KilledByEnemy = 7,
    };
}
namespace app::ropeway::ExtraContents {
    enum class Type : int64_t {
        Rogue = 0,
        The4th = 1,
        Tofu = 2,
        ClassicBGM = 3,
        UnlockAllBonus = 4,
        Costume_A = 5,
        Costume_B = 6,
        Costume_C = 7,
        Costume_D = 8,
        Costume_E = 9,
        Costume_F = 10,
        Costume_G = 11,
        Weapon_A = 12,
        Weapon_B = 13,
        Weapon_C = 14,
        Player_A = 15,
        Player_B = 16,
        ChineseVoice = 17,
    };
}
namespace app::ropeway::behaviortree::condition::survivor::npc::NpcBtCondition_UniqueCheck {
    enum class CheckConditionKind : int64_t {
        None = 0,
        PL3000_EnableCrouch = 1,
        PL3000_FourceCrouch = 2,
    };
}
namespace app::ropeway::LoadCollisionType {
    enum class Type : int64_t {
        LOAD = 0,
        UNLOAD = 1,
    };
}
namespace via::hid {
    enum class KeyboardSpecialKey : int64_t {
        None = 0,
        Shift = 16,
        Control = 17,
        Menu = 18,
    };
}
namespace via::physics::StaticCompoundShape::Instance {
    enum class Type : int64_t {
        Additive = 0,
        Subtractive = 1,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0200_MONTAGE_PARTS_HAT : int64_t {
        HAT00 = 0,
        NONE = 1,
    };
}
namespace app::ropeway::environment::lifecycle::GimmickSetManagerBridgeSub {
    enum class ActionType : int64_t {
        ACTIVATE = 0,
        DEACTIVATE = 1,
    };
}
namespace via {
    enum class AreaQueryCategory : int64_t {
        Unknown = 0,
        AreaItem = 1,
        Position = 2,
        GameObject = 4,
        AreaRegion = 8,
        Any = 255,
    };
}
namespace app::ropeway::enemy::em7100 {
    enum class MotionCoreId : int64_t {
        CORE_ID_BASE = 0,
        CORE_ID_ADD_DAMAGE = 1,
        CORE_ID_EYE = 2,
        CORE_ID_WILLIAM = 3,
        CORE_ID_BACK_EYE = 4,
    };
}
namespace via::motion::Motion {
    enum class IntervalUpdateOption : int64_t {
        None = 0,
        ApplyAllJoints = 1,
        UpdateRootAndApplyRoot = 2,
        UpdateRootAndApplyAllJoints = 3,
    };
}
namespace app::ropeway::enemy::tracks::Em7300ColliderTrack {
    enum class DecalGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Decal_AttackBurst_L = 12,
        Decal_AttackBurst_R = 13,
        Decal_AttackToss_L = 14,
        Decal_AttackToss_R = 15,
    };
}
namespace via::network::session {
    enum class Region : int64_t {
        Region_0 = 0,
        Region_1 = 1,
        Region_2 = 2,
        Region_3 = 3,
        Region_Max = 4,
    };
}
namespace app::ropeway::gui::GimmickNumberLockHothouseGuiBehavior2 {
    enum class RoutineType : int64_t {
        WAIT = 0,
        SELECT = 1,
        FAILURE = 2,
        SUCCESS = 3,
    };
}
namespace via::geometry::GeometryBrush {
    enum class BooleanState : int64_t {
        No = 0,
        ReadyNo = 1,
        ReadyYes = 2,
        Yes = 3,
    };
}
namespace app::ropeway::TwoBoneIK {
    enum class TargetMode : int64_t {
        GameObject = 0,
        Position = 1,
    };
}
namespace via::render::LDRColorDeficiencySimulation {
    enum class DeficiencyType : int64_t {
        Normal = 0,
        Protanopia = 1,
        Deuteranopia = 2,
        Tritanopia = 3,
        Achromatopsia = 4,
        Max = 5,
    };
}
namespace via::navigation {
    enum class PathObjectOperatorType : int64_t {
        PathObjectOperator_NavigationTargetStraight = 0,
        PathObjectOperator_NavMeshStraight = 1,
        PathObjectOperator_NavigationVolumeSpaceTargetStraight = 2,
        PathObjectOperator_NavigationVolumeSpace = 3,
        PathObjectOperator_NavigationWaypointTargetStraight = 4,
        PathObjectOperator_NavigationWaypoint = 5,
        PathObjectOperatorNum = 6,
    };
}
namespace app::ropeway::enemy::em6300::MotionPattern {
    enum class DirFBLR : int64_t {
        Front = 0,
        Back = 1,
        Left = 2,
        Right = 3,
    };
}
namespace app::ropeway::Em4000Define {
    enum class ActionType : int64_t {
        Normal = 0,
        Fall = 1,
        Dig = 2,
        Eat = 3,
        LieDown = 4,
        Hole = 5,
        Barricade = 6,
        Jump = 7,
        CarClimb = 8,
    };
}
namespace app::ropeway::gamemastering::MainFlowManager {
    enum class TofuType : int64_t {
        A = 0,
        B = 1,
        C = 2,
        D = 3,
        E = 4,
        MAX = 5,
    };
}
namespace app::ropeway::fsmv2::condition::FsmCondition_CompareValueBase {
    enum class CompareModeType : int64_t {
        Equal = 0,
        NotEqual = 1,
    };
}
namespace via {
    enum class InterpolationType : int64_t {
        Unknown = 0,
        Discrete = 1,
        Linear = 2,
        Event = 3,
        Slerp = 4,
        Hermite = 5,
        AutoHermite = 6,
        Bezier = 7,
        AutoBezier = 8,
        OffsetFrame = 9,
        OffsetSec = 10,
        PassEvent = 11,
        Bezier3D = 12,
        Range = 13,
    };
}
namespace app::ropeway::gimmick::action::GimmickCustomJackControl::ActorWork {
    enum class UpdateWorkState : int64_t {
        UNSET = 0,
        LOADING = 1,
        STAND_BY = 2,
        PLAYING = 3,
        RESET = 4,
        AFTER = 5,
        END = 6,
    };
}
namespace app::ropeway::network::service::ReNetServiceController {
    enum class Phase : int64_t {
        Entry = 0,
        Requested = 1,
        Setup = 2,
        WaitSetup = 3,
        AuthCode = 4,
        WaitAuthCode = 5,
        Post = 6,
        WaitResponse = 7,
        Failed = 8,
    };
}
namespace via::render {
    enum class LightMask : int64_t {
        Default = 0,
        Beauty = 1,
    };
}
namespace app::ropeway::survivor::player::PlayerFacialController {
    enum class EmotionStatusMode : int64_t {
        NONE = 0xFFFFFFFF,
        DAMAGED_CAUTION = 0,
        DAMAGED_DANGER = 1,
        NUM = 2,
    };
}
namespace via::render::LightVolumeDecal {
    enum class DecalTextureSize : int64_t {
        DecalTextureSize_64 = 64,
        DecalTextureSize_128 = 128,
        DecalTextureSize_256 = 256,
        DecalTextureSize_512 = 512,
        DecalTextureSize_1K = 1024,
    };
}
namespace app::ropeway::MainSceneUpdateManager {
    enum class InstantiatePhase : int64_t {
        ContextSetup = 0,
        Execution = 1,
    };
}
namespace app::ropeway::gamemastering::MovieManager {
    enum class PauseExecState : int64_t {
        Idle = 0,
        ExecPause = 1,
        ExecPauseAfter = 2,
        SkipWwise = 3,
        SkipEvent = 4,
        CalcTimeLag = 5,
    };
}
namespace app::ropeway::FileLocation {
    enum class ID : int64_t {
        invalid = 0,
        RPD = 1,
        OrphanAsylum = 2,
        WasteWater = 3,
        Laboratory = 4,
        Etcetera = 5,
        Unknown = 6,
        Tutorial = 7,
    };
}
namespace via::telemetry::TelemetryManager {
    enum class EventType : int64_t {
        Ready = 0,
        Activated = 0,
        Deactivated = 1,
    };
}
namespace via::effect::script::EPVExpertObjectLandingData {
    enum class ZDirectionType : int64_t {
        CollisionNormal = 0,
        InverseCollisionNormal = 1,
        AttackDirection = 2,
        InverseAttackDirection = 3,
        SawRotation = 4,
    };
}
namespace app::ropeway::LocationThroughManager {
    enum class ProcessStep : int64_t {
        INITIAL_LOAD_INIT = 0,
        INITIAL_LOAD_WAIT = 1,
        IDLE = 2,
        JUMP_FADEOUT_WAIT = 3,
        JUMP_LOAD_LOCATION = 4,
        JUMP_LOAD_MAP = 5,
        JUMP_LOAD_POSITIONING = 6,
    };
}
namespace app::ropeway::enemy::common::EnemyMaterialAnimator::HPColorData {
    enum class eVariableType : int64_t {
        None = 0,
        Float = 1,
        Float4 = 2,
    };
}
namespace app::ropeway::gamemastering::RoguePlayerManager {
    enum class State : int64_t {
        INVALID = 0xFFFFFFFF,
        INITIALIZE = 0,
        WAIT_TO_INGAME = 1,
        INITIALIZE_TO_INGAME = 2,
        INGAME = 3,
        ERROR_ = 4,
    };
}
namespace via {
    enum class EffectPerformanceLevel : int64_t {
        Low = 0,
        Normal = 1,
        Caution = 2,
        Middle = 3,
        High = 4,
    };
}
namespace app::ropeway::timeline::TimelineEventSettingTable::FadeSetting::FadeParam {
    enum class FadeType : int64_t {
        Nothing = 0,
        FadeIN_ = 1,
        FadeOut = 2,
    };
}
namespace app::ropeway::gui::ItemPortableSafeBehavior {
    enum class DemoStateEnum : int64_t {
        LampAllOn = 0,
        LampEvenNumberOn = 1,
        LampOddNumberOn = 2,
        ButtonAllOff = 3,
    };
}
namespace app::ropeway::CollisionSeDefine {
    enum class ATK_HIT_TYPE : int64_t {
        ATK_HIT_S = 0,
        ATK_HIT_M = 1,
        ATK_HIT_L = 2,
        ATK_HIT_DEAD = 3,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class WwiseEnemyAttentionVolume : int64_t {
        ATTENTION_LOST = 0,
        ATTENTION_1_BODY = 1,
        ATTENTION_2_BODY = 2,
        ATTENTION_3_BODY = 3,
        ATTENTION_4_ANDMORE_BODY = 4,
    };
}
namespace via::navigation::map::MapData {
    enum class MapStructure : int64_t {
        Unclassified = 0,
        GroundBase = 1,
        AllSurface = 2,
    };
}
namespace app::ropeway::gamemastering::MainFlowManager {
    enum class Difficulty : int64_t {
        EASY = 0,
        NORMAL = 1,
        HARD = 2,
        MAX = 3,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0100_MONTAGE_PARTS_PANTS : int64_t {
        NONE = 0,
        PANTS00 = 1,
        PANTS01 = 2,
        PANTS01_00_00 = 3,
        PANTS01_01_00 = 4,
        PANTS02 = 5,
        PANTS04 = 6,
    };
}
namespace app::ropeway::SurvivorDefine::State {
    enum class TotalBody : int64_t {
        STAND = 0,
        GAZING_WALKING = 1,
        QUICK_TURN = 2,
        JOG_START = 3,
        JOGGING = 4,
        HOLD_START = 5,
        HOLDING = 6,
        HOLD_WHEEL = 7,
        HOLD_WALKING = 8,
        FOCUS_HOLDING = 9,
        FOCUS_HOLD_WALKING = 10,
        SUPPORT_HOLDING = 11,
        SUPPORT_HOLD_WALKING = 12,
        SUPPORT_HOLD_SHOOT = 13,
        KNIFE_ATTACK = 14,
        KNIFE_ATTACK_STAB = 15,
        KNIFE_ATTACK_STAB_ADD = 16,
        KNIFE_ATTACK_STAB_IDLE = 17,
        REACTION = 18,
        JACK = 19,
        EVENT = 20,
    };
}
namespace app::ropeway::weapon::generator {
    enum class OwnerType : int64_t {
        Invalid = 0xFFFFFFFF,
        Survivor = 0,
        Weapon = 1,
        Enemy = 2,
        Gimmick = 3,
    };
}
namespace via::effect::detail {
    enum class DivideAxisType : int64_t {
        X = 0,
        Y = 1,
        Z = 2,
    };
}
namespace app::ropeway::File {
    enum class ID : int64_t {
        Invalid = 0,
        Mes_File_01 = 1,
        Mes_File_02 = 2,
        Mes_File_03 = 3,
        Mes_File_04 = 4,
        Mes_File_05 = 5,
        Mes_File_06 = 6,
        Mes_File_07 = 7,
        Mes_File_08 = 8,
        Mes_File_09 = 9,
        Mes_File_10 = 10,
        Mes_File_11 = 11,
        Mes_File_12 = 12,
        Mes_File_13 = 13,
        Mes_File_14 = 14,
        Mes_File_15 = 15,
        Mes_File_16 = 16,
        Mes_File_17 = 17,
        Mes_File_18 = 18,
        Mes_File_19 = 19,
        Mes_File_20 = 20,
        Mes_File_21 = 21,
        Mes_File_22 = 22,
        Mes_File_23 = 23,
        Mes_File_24 = 24,
        Mes_File_25 = 25,
        Mes_File_26 = 26,
        Mes_File_27 = 27,
        Mes_File_28 = 28,
        Mes_File_29 = 29,
        Mes_File_30 = 30,
        Mes_File_31 = 31,
        Mes_File_32 = 32,
        Mes_File_33 = 33,
        Mes_File_34 = 34,
        Mes_File_35 = 35,
        Mes_File_36 = 36,
        Mes_File_37 = 37,
        Mes_File_38 = 38,
        Mes_File_39 = 39,
        Mes_File_40 = 40,
        Mes_File_41 = 41,
        Mes_File_42 = 42,
        Mes_File_43 = 43,
        Mes_File_44 = 44,
        Mes_File_45 = 45,
        Mes_File_46 = 46,
        Mes_File_47 = 47,
        Mes_File_48 = 48,
        Mes_File_49 = 49,
        Mes_File_50 = 50,
        Mes_File_51 = 51,
        Mes_File_52 = 52,
        Mes_File_53 = 53,
        Mes_File_54 = 54,
        Mes_File_55 = 55,
        Mes_File_56 = 56,
        Mes_File_57 = 57,
        Mes_File_58 = 58,
        Mes_File_59 = 59,
        Mes_File_60 = 60,
        Mes_File_61 = 61,
        Mes_File_62 = 62,
        Mes_File_63 = 63,
        Mes_File_64 = 64,
        Mes_File_65 = 65,
        Mes_File_66 = 66,
        Mes_File_67 = 67,
        Mes_File_68 = 68,
        Mes_File_69 = 69,
        Mes_File_70 = 70,
        Mes_File_71 = 71,
        Mes_File_72 = 72,
        Mes_File_73 = 73,
        Mes_File_74 = 74,
        Mes_File_75 = 75,
        Mes_File_76 = 76,
        Mes_File_77 = 77,
        Mes_File_78 = 78,
        Mes_File_79 = 79,
        Mes_File_80 = 80,
        Mes_File_81 = 81,
        Mes_File_82 = 82,
        Mes_File_83 = 83,
        Mes_File_84 = 84,
        Mes_File_85 = 85,
        Mes_File_86 = 86,
        Mes_File_87 = 87,
        Mes_File_88 = 88,
        Mes_File_89 = 89,
        MAX = 90,
    };
}
namespace via::eq::Token {
    enum class Type : int64_t {
        Constant = 0,
        Operator = 1,
        UnaryOperator = 2,
        Function = 3,
        Variable = 4,
    };
}
namespace via::effect::detail {
    enum class EffectBoundsType : int64_t {
        None = 0,
        Sphere = 1,
        AABB = 2,
        OBB = 3,
    };
}
namespace via::navigation::AIMapAutoScan {
    enum class AutoState : int64_t {
        Move = 0,
        FromCenter = 1,
        ToCenter = 2,
        MoveRoot = 3,
    };
}
namespace app::ropeway::gui::CostumeBehavior {
    enum class State : int64_t {
        MAIN_ = 0,
        SUB = 1,
        WAIT_SAVE = 2,
        MAX = 3,
        INVALID = 4,
    };
}
namespace app::ropeway::supportitem::SupportItemAttachmentController {
    enum class EmissiveType : int64_t {
        OnAttach = 0,
        OnDetach = 1,
        ActionCall = 2,
    };
}
namespace app::ropeway::AssetPackagePair {
    enum class GimmickType : int64_t {
        UNSET = 0xFFFFFFFF,
        WINDOW = 0,
        DOOR = 1,
        LADDER = 2,
        JUMP_DOWN = 3,
        LEVER_NORMAL = 4,
        LEVER_BUTTON = 5,
        LEVER_PULL = 6,
        BOOKSHELF = 7,
        BOX_S = 8,
        LEVER_G2CRANE = 9,
        ROGUE_WARPGATE = 10,
        RAISESHELF = 11,
        RAISESHELF_MOVE = 12,
        FENCEJUMP = 13,
        MAX = 14,
    };
}
namespace app::ropeway::survivor::npc::moveline::Point {
    enum class PositionSettingTypeKind : int64_t {
        Position = 0,
        GameObject = 1,
    };
}
namespace via::hid {
    enum class NpadJoyHoldType : int64_t {
        Vertical = 0,
        Horizontal = 1,
    };
}
namespace app::ropeway::gui::NewInventoryBehavior {
    enum class Mode : int64_t {
        Initialize = 0,
        CallOpen = 1,
        CallOpenGimmick = 2,
        EnterInventory = 3,
        CallClose = 4,
        LeaveInventory = 5,
        CallOpenDetail = 6,
        FinishDetail = 7,
        CallPickUpFirst = 8,
        CallPickUpFirstReady = 9,
        FinishPickUpFirst = 10,
    };
}
namespace via::hid::virtualKeyboard::nsw {
    enum class WindowMode : int64_t {
        Single = 0,
        Floating = 1,
        AdjacentFloating = 2,
    };
}
namespace app::ropeway::gui::RogueInventoryBehavior {
    enum class GuideType : int64_t {
        NoDisp = 0,
        Map = 1,
        File = 2,
    };
}
namespace app::ropeway::AimUserDataBase {
    enum class TypeKind : int64_t {
        Target = 0,
        Forbid = 1,
    };
}
namespace via::physics::CollisionSkinningMeshResource {
    enum class WeightType : int64_t {
        None = 0,
        Weight4 = 1,
        Weight8 = 2,
    };
}
namespace via::physics::RequestSetCollider::RequestSet {
    enum class State : int64_t {
        Registered = 0,
        Max = 1,
    };
}
namespace app::ropeway::NaviMoveSupporter {
    enum class StopReasonType : int64_t {
        Invalid = 0xFFFFFFFF,
        Arrival = 0,
        Faied = 1,
    };
}
namespace via {
    enum class DisplayType : int64_t {
        Fit = 0,
        Uniform4x3 = 1,
        Uniform16x9 = 2,
        Uniform16x10 = 3,
        Uniform21x9 = 4,
        Uniform32x9 = 5,
        Uniform48x9 = 6,
        Fix480p = 7,
        Fix720p = 8,
        Fix1080p = 9,
        Fix4K = 10,
        Fix8K = 11,
        Fix12K = 12,
        Fix16K = 13,
        FixResolution = 14,
        FixResolution16x9 = 15,
        NintendoSwitch = 16,
        Fix684x384 = 17,
        Fix1368x768 = 18,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class Unexpected : int64_t {
        Default = 121,
        Abstract = 122,
        Setting = 1000,
        Tool = 2,
        DevelopOnly = 3,
        Undefined = 4,
        DevelopLog = 5,
    };
}
namespace via::effect::detail {
    enum class Shape3DType : int64_t {
        Box = 0,
        Sphere = 1,
        Cylinder = 2,
    };
}
namespace app::ropeway::EnemyManager {
    enum class SceneLoadEventType : int64_t {
        Invalid = 0,
        Standby = 1,
        Activate = 2,
        Deactivate = 3,
        Release = 4,
    };
}
namespace app::ropeway::enemy::em7000::Em7000Think {
    enum class BackGrappleType : int64_t {
        BackGrapple_Type_A = 0,
        BackGrapple_Type_B = 1,
    };
}
namespace via::render {
    enum class WindowModeChangeStatus : int64_t {
        SetWindowMode = 0,
        KeyboardShortcuts = 1,
        Occlude = 2,
        AutoReturnFullScreen = 3,
    };
}
namespace via::timeline::UserCurvePlayer {
    enum class ResourceType : int64_t {
        None = 0,
        UserCurve = 1,
        UserCurveList = 2,
    };
}
namespace via::render {
    enum class BufferUavFlag : int64_t {
        Raw = 1,
    };
}
namespace app::ropeway::SurvivorDefine::ActionOrder {
    enum class Petient : int64_t {
        INVALID = 0,
        IDLE = 1,
        LIGHT_WHEEL = 2,
        MOVE = 4,
        JOG = 8,
        HOLD = 16,
        HOLD_WHEEL = 32,
        HOLD_WALK = 64,
    };
}
namespace via::storage::saveService {
    enum class SaveIcon : int64_t {
        IconNew = 0,
        Icon1 = 1,
        Icon2 = 2,
        Icon3 = 3,
        Icon4 = 4,
        Icon5 = 5,
        Icon6 = 6,
        Icon7 = 7,
        Icon8 = 8,
        Icon9 = 9,
        Icon10 = 10,
        Icon11 = 11,
        Icon12 = 12,
        Icon13 = 13,
        Icon14 = 14,
        Icon15 = 15,
        Icon16 = 16,
        Icon17 = 17,
        Icon18 = 18,
        Icon19 = 19,
        Icon20 = 20,
        Icon21 = 21,
        Icon22 = 22,
        Icon23 = 23,
        Icon24 = 24,
        Icon25 = 25,
        Icon26 = 26,
        Icon27 = 27,
        Icon28 = 28,
        Icon29 = 29,
        Icon30 = 30,
        Icon31 = 31,
        Icon32 = 32,
        Icon33 = 33,
        Icon34 = 34,
        Icon35 = 35,
        Icon36 = 36,
        Icon37 = 37,
        Icon38 = 38,
        Icon39 = 39,
        Icon40 = 40,
        Icon41 = 41,
        Icon42 = 42,
        Icon43 = 43,
        Icon44 = 44,
        Icon45 = 45,
        Icon46 = 46,
        Icon47 = 47,
        Icon48 = 48,
        Icon49 = 49,
        Icon50 = 50,
        Icon51 = 51,
        Icon52 = 52,
        Icon53 = 53,
        Icon54 = 54,
        Icon55 = 55,
        Icon56 = 56,
        Icon57 = 57,
        Icon58 = 58,
        Icon59 = 59,
        Icon60 = 60,
        Icon61 = 61,
        Icon62 = 62,
        Icon63 = 63,
        Icon64 = 64,
        Icon65 = 65,
        Icon66 = 66,
        Icon67 = 67,
        Icon68 = 68,
        Icon69 = 69,
        Icon70 = 70,
        Icon71 = 71,
        Icon72 = 72,
        Icon73 = 73,
        Icon74 = 74,
        Icon75 = 75,
        Icon76 = 76,
        Icon77 = 77,
        Icon78 = 78,
        Icon79 = 79,
        Icon80 = 80,
        Icon81 = 81,
        Icon82 = 82,
        Icon83 = 83,
        Icon84 = 84,
        Icon85 = 85,
        Icon86 = 86,
        Icon87 = 87,
        Icon88 = 88,
        Icon89 = 89,
        Icon90 = 90,
        Icon91 = 91,
        Icon92 = 92,
        Icon93 = 93,
        Icon94 = 94,
        Icon95 = 95,
        Icon96 = 96,
        Icon97 = 97,
        Icon98 = 98,
        Icon99 = 99,
        IconMax = 100,
    };
}
namespace app::ropeway::enemy::em0000::RoleAction {
    enum class WindowSet : int64_t {
        IDLE_TO_KNOCK = 0,
        IDLE_TO_KNOCK_WITH_MARK = 1,
        KNOCK = 2,
        IDLE = 3,
    };
}
namespace app::ropeway::InputDefine {
    enum class PadKind : int64_t {
        INVALID = 0,
        USER_0 = 1,
        USER_1 = 2,
        USER_2 = 3,
        USER_3 = 4,
    };
}
namespace app::ropeway::enemy::em3000::tracks::Em3000SetGroundStateTrack {
    enum class STATE : int64_t {
        INVALID = 0,
        JUMP = 1,
        GROUND = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0000_MONTAGE_PARTS_BODY : int64_t {
        BODY00 = 0,
        BODY01 = 1,
        BODY02 = 2,
        BODY03 = 3,
        BODY04 = 4,
        BODY06 = 5,
        BODY07 = 6,
        BODY08 = 7,
        BODY70 = 8,
        BODY71 = 9,
        BODY72 = 10,
        BODY74 = 11,
        BODY75 = 12,
        BODY76 = 13,
        NONE = 14,
    };
}
namespace via::storage::saveService {
    enum class SaveDataArrayType : int64_t {
        Value = 0,
        Class = 1,
    };
}
namespace via::wwise {
    enum class OutputDevice : int64_t {
        None = 0,
        HDMI = 1,
        Headphone = 2,
        Speaker = 3,
        Max = 4,
    };
}
namespace app::ropeway::enemy::em6000::fsmv2::action::Em6000FsmAction_WaitTime {
    enum class TimerKind : int64_t {
        Ambush = 0,
        FillInIdle = 1,
        SwimHold = 2,
        EyeDown = 3,
    };
}
namespace via::gui {
    enum class FontSlot : int64_t {
        Slot0 = 0,
        Slot1 = 1,
        Slot2 = 2,
        Slot3 = 3,
        Slot4 = 4,
        Slot5 = 5,
        Slot6 = 6,
        Slot7 = 7,
        Slot8 = 8,
        Slot9 = 9,
        Max = 10,
    };
}
namespace via::render {
    enum class Filter : int64_t {
        MinMagMipPoint = 0,
        MinMagPointMipLinear = 1,
        MinPointMagLinearMipPoint = 4,
        MinPointMagMipLinear = 5,
        MinLinearMagMipPoint = 16,
        MinLinearMagPointMipLinear = 17,
        MinMagLinearMipPoint = 20,
        MinMagMipLinear = 21,
        Anisotropic = 85,
        ComparisonMinMagMipPoint = 128,
        ComparisonMinMagPointMipLinear = 129,
        ComparisonMinPointMagLinearMipPoint = 132,
        ComparisonMinPointMagMipLinear = 133,
        ComparisonMinLinearMagMipPoint = 144,
        ComparisonMinLinearMagPointMipLinear = 145,
        ComparisonMinMagLinearMipPoint = 148,
        ComparisonMinMagMipLinear = 149,
        ComparisonAnisotropic = 213,
    };
}
namespace app::ropeway::enemy::em6200::Em6200Think {
    enum class CameraSeTypeEnum : int64_t {
        RangeIN_ = 0,
        RangeOver = 1,
    };
}
namespace via::render {
    enum class EyeballShadingType : int64_t {
        Standard = 0,
        Expensive = 1,
        ExpensiveWithCaustics = 2,
    };
}
namespace app::ropeway::enemy::em6200::Em6200Think {
    enum class FLAG : int64_t {
        LookAt = 0,
        AidaMode = 1,
        SherryMode = 2,
        FourthMode = 3,
        AttackHited = 4,
        DamageHited = 5,
        FirstConvReady = 6,
        RevivalConvReady = 7,
        Provoke = 8,
        OnHat = 9,
        DoorOverNaviCheck = 10,
        GhostNaviMove = 11,
        DoorPathMovePos = 12,
        WaistCrouch = 13,
        GrappleHandAdd = 14,
        AlwaysFind = 15,
        ForceFind = 16,
        NarrowRequest = 17,
    };
}
namespace app::ropeway::survivor::motion::SurvivorTargetBankController::WeaponCustomBitElement {
    enum class CustomKind : int64_t {
        A = 0,
        B = 1,
        C = 2,
    };
}
namespace app::ropeway::enemy::em6200 {
    enum class RankParamHash : int64_t {
        MOVE_SPEED_SCALE = 225628402,
        GHOST_MOVE_SPEED_SCALE = 1602663712,
        PROVOKE_RATE = 1474837118,
        GRAPPLE_COMBO_RATE = 3796966406,
    };
}
namespace via::gui::GUIUtility {
    enum class AnalyzeContext : int64_t {
        Key = 0,
        Association = 1,
        Value = 2,
        SearchNextKey = 3,
        Unknown = 4,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class Em0000DirtyPreset_Shirt : int64_t {
        Shirt00 = 0,
        Shirt01 = 1,
        Shirt02 = 2,
        Shirt03 = 3,
        Shirt04 = 4,
        Shirt05 = 5,
        Shirt06 = 6,
        Shirt07 = 7,
        Shirt08 = 8,
        Shirt09 = 9,
        Shirt10 = 10,
    };
}
namespace app::ropeway::enemy::common::behaviortree::action::EmCommonBtAction_SetTarget {
    enum class TYPE : int64_t {
        Hate = 0,
        Player = 1,
        Clear = 2,
    };
}
namespace via::fsm {
    enum class StateQueryType : int64_t {
        All = 0,
        Start = 1,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class WayPointAttribute : int64_t {
        LockedDoor = 2,
        SafeRoom = 4,
    };
}
namespace app::ropeway::CharacterDefine {
    enum class MarkerReactionType : int64_t {
        None = 0,
        Heat = 1,
        Burn = 2,
        Cold = 4,
    };
}
namespace app::ropeway::effect::script::EffectRecordSave {
    enum class SaveTypes : int64_t {
        Template = 0,
        Scar = 1,
        Burn = 2,
        Acid = 3,
    };
}
namespace via::clr::VMRuntimeType {
    enum class Kind : int64_t {
        Default = 0,
        GenericParameter = 1,
        ByRef = 2,
        Pointer = 3,
    };
}
namespace via::memory {
    enum class InstanceType : int64_t {
        Default = 0,
        Object = 1,
        ManagedObject = 2,
    };
}
namespace via::render::detail {
    enum class PrimitiveDistortion : int64_t {
        Blur = 0,
        Refract = 1,
    };
}
namespace app::ropeway::gui::RogueGeneralAnnounceBehavior {
    enum class RogueAnnounceType : int64_t {
        INVALID = 0xFFFFFFFF,
        NONE = 0,
        LVUP = 1,
        BOSS = 2,
        GAME_START = 3,
        GAME_FAILED = 4,
        GAME_CLEAR = 5,
    };
}
namespace app::ropeway::enemy::tracks::Em7300ColliderTrack {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Attack_SecondArmL = 3,
        Attack_SecondArmR = 4,
        AttackBurst_L = 7,
        AttackBurst_R = 8,
        AttackToss_L = 9,
        AttackToss_R = 10,
        Attack_Dash_Start = 36,
        Attack_Train_Landing = 37,
        Attack_Wall_Landing_Outside = 38,
        Attack_Wall_Landing_Center = 39,
        Attack_Train_Landing_Outside = 40,
        AttackDeadDown_L = 41,
        AttackDeadDown_R = 42,
    };
}
namespace app::ropeway::gui::SelectBrightnessBehaviorMax {
    enum class EndType : int64_t {
        SELECTED = 0,
        CANCELED = 1,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class WalkKindID : int64_t {
        TYPE_A = 0,
        TYPE_B = 1,
        TYPE_C = 2,
        TYPE_D = 3,
        TYPE_E = 4,
        TYPE_F = 5,
    };
}
namespace via::render::layer::Transparent {
    enum class LuminanceBleedType : int64_t {
        Pre = 0,
        Post = 1,
    };
}
namespace via::behaviortree::TreeNode {
    enum class SelectorStatus : int64_t {
        Off = 0,
        Start = 1,
        Work = 2,
    };
}
namespace via::hid {
    enum class DeviceCaps : int64_t {
        None = 0,
        ForceFeedback = 1,
        TouchInterface = 2,
        GamePadButtonCLeft = 4,
        GamePadButtonCCenter = 8,
        GamePadButtonCRight = 16,
        MotionSensor = 32,
    };
}
namespace app::ropeway::RogueGUIMaster {
    enum class RogueState : int64_t {
        Invalid = 0xFFFFFFFF,
        ExtraMenu = 0,
        BaseArea = 1,
        InGame = 2,
        Result = 3,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::ForceRequestActionRoot {
    enum class ExecType : int64_t {
        Start = 0,
        Exit = 1,
        EndOfMotion = 2,
    };
}
namespace app::ropeway::gamemastering::ResultFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        WAIT_GUI_CREATION = 1,
        UPDATE = 2,
        WAIT_FADE = 3,
        FINALIZE = 4,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace app::ropeway::enemy::em4400 {
    enum class ESCAPE_TYPE : int64_t {
        NONE = 0,
        POINT = 1,
        RANGE_AWAY = 2,
        COUNTER_FLY = 3,
    };
}
namespace via::render::Bloodshed {
    enum class SourceChannel : int64_t {
        R = 1,
        G = 2,
        B = 3,
        A = 4,
    };
}
namespace app::ropeway::gamemastering::GameMaster {
    enum class ServicePlatformType : int64_t {
        INVALID = 0,
        DEFAULT = 1,
        STEAM = 2,
        UWP = 3,
        WEGAME = 4,
    };
}
namespace via::motion::JointExMultiRemapValue {
    enum class RotOrder : int64_t {
        XYZ = 0,
        YZX = 1,
        ZXY = 2,
        ZYX = 3,
        YXZ = 4,
        XZY = 5,
    };
}
namespace System::Globalization {
    enum class DateTimeStyles : int64_t {
        None = 0,
        AllowLeadingWhite = 1,
        AllowTrailingWhite = 2,
        AllowInnerWhite = 4,
        AllowWhiteSpaces = 7,
        NoCurrentDateDefault = 8,
        AdjustToUniversal = 16,
        AssumeLocal = 32,
        AssumeUniversal = 64,
        RoundtripKind = 128,
    };
}
namespace via::render {
    enum class SupportRenderMode : int64_t {
        DeferredAndForward = 0,
        Deferred = 1,
        Forward = 2,
    };
}
namespace via {
    enum class HashType : int64_t {
        CRC16 = 0,
        CRC32 = 1,
        MD2 = 2,
        MD4 = 3,
        MD5 = 4,
        SHA1 = 5,
        SHA256 = 6,
        SHA384 = 7,
        SHA512 = 8,
        RIPEMD128 = 9,
        RIPEMD160 = 10,
        RIPEMD256 = 11,
        RIPEMD320 = 12,
    };
}
namespace via::movie::Movie {
    enum class CosmeticState : int64_t {
        Preparing = 0,
        Ready = 1,
        Playing = 2,
        Paused = 3,
        Finished = 4,
    };
}
namespace via::areamap::AreaTest {
    enum class Mode : int64_t {
        Score = 0,
        Filter = 1,
        Both = 2,
    };
}
namespace via::movie {
    enum class PlaybackPerformance : int64_t {
        Slowest = 0,
        Normal = 1,
        Fastest = 2,
    };
}
namespace app::ropeway::EquipmentDefine {
    enum class EquipCategory : int64_t {
        Invalid = 0xFFFFFFFF,
        MaIN_ = 0,
        Sub = 1,
    };
}
namespace app::ropeway::CameraTwirler {
    enum class UpdateType : int64_t {
        INVALID = 0,
        UP = 1,
        DOWN = 2,
    };
}
namespace app::ropeway::enemy::common::behaviortree::condition::CheckHp {
    enum class ExpressionType : int64_t {
        Immediate = 0,
        Rate = 1,
    };
}
namespace via::hid::VrTrackerResultData {
    enum class ProjectionQuality : int64_t {
        Raw = 0,
        None = 3,
        Partial = 6,
        Full = 9,
    };
}
namespace via::render {
    enum class ShadowCastMode : int64_t {
        A = 1,
        B = 2,
    };
}
namespace app::ropeway::gamemastering::RecordManager {
    enum class RewardId : int64_t {
        INVALID = 0,
        OPEN_MODE_00 = 1,
        OPEN_MODE_01 = 2,
        OPEN_MODE_02 = 3,
        OPEN_MODE_03 = 4,
        OPEN_MODE_04 = 5,
        OPEN_MODE_05 = 6,
        OPEN_MODE_06 = 7,
        OPEN_MODE_07 = 8,
        OPEN_MODE_08 = 9,
        OPEN_MODE_09 = 10,
        COSTUME_00 = 11,
        COSTUME_01 = 12,
        COSTUME_02 = 13,
        COSTUME_03 = 14,
        COSTUME_04 = 15,
        COSTUME_05 = 16,
        COSTUME_06 = 17,
        COSTUME_07 = 18,
        COSTUME_08 = 19,
        WEAPON_00 = 20,
        WEAPON_01 = 21,
        WEAPON_02 = 22,
        WEAPON_03 = 23,
        WEAPON_04 = 24,
        CONCEPTART_00 = 25,
        CONCEPTART_01 = 26,
        CONCEPTART_02 = 27,
        CONCEPTART_03 = 28,
        CONCEPTART_04 = 29,
        CONCEPTART_05 = 30,
        CONCEPTART_06 = 31,
        CONCEPTART_07 = 32,
        CONCEPTART_08 = 33,
        CONCEPTART_09 = 34,
        CONCEPTART_10 = 35,
        CONCEPTART_11 = 36,
        CONCEPTART_12 = 37,
        CONCEPTART_13 = 38,
        CONCEPTART_14 = 39,
        CONCEPTART_15 = 40,
        CONCEPTART_16 = 41,
        CONCEPTART_17 = 42,
        CONCEPTART_18 = 43,
        CONCEPTART_19 = 44,
        CONCEPTART_20 = 45,
        CONCEPTART_21 = 46,
        CONCEPTART_22 = 47,
        CONCEPTART_23 = 48,
        CONCEPTART_24 = 49,
        CONCEPTART_25 = 50,
        CONCEPTART_26 = 51,
        CONCEPTART_27 = 52,
        CONCEPTART_28 = 53,
        CONCEPTART_29 = 54,
        FIGURE_00 = 55,
        FIGURE_01 = 56,
        FIGURE_02 = 57,
        FIGURE_03 = 58,
        FIGURE_04 = 59,
        FIGURE_05 = 60,
        FIGURE_06 = 61,
        FIGURE_07 = 62,
        FIGURE_08 = 63,
        FIGURE_09 = 64,
        FIGURE_10 = 65,
        FIGURE_11 = 66,
        FIGURE_12 = 67,
        FIGURE_13 = 68,
        FIGURE_14 = 69,
        FIGURE_15 = 70,
        FIGURE_16 = 71,
        FIGURE_17 = 72,
        FIGURE_18 = 73,
        FIGURE_19 = 74,
        FIGURE_20 = 75,
        FIGURE_21 = 76,
        FIGURE_22 = 77,
        FIGURE_23 = 78,
        FIGURE_24 = 79,
        FIGURE_25 = 80,
        FIGURE_26 = 81,
        FIGURE_27 = 82,
        FIGURE_28 = 83,
        FIGURE_29 = 84,
        FIGURE_30 = 85,
        FIGURE_31 = 86,
        FIGURE_32 = 87,
        FIGURE_33 = 88,
        FIGURE_34 = 89,
        FIGURE_35 = 90,
        FIGURE_36 = 91,
        FIGURE_37 = 92,
        FIGURE_38 = 93,
        FIGURE_39 = 94,
        FIGURE_40 = 95,
        FIGURE_41 = 96,
        FIGURE_42 = 97,
        FIGURE_43 = 98,
        FIGURE_44 = 99,
        FIGURE_45 = 100,
        FIGURE_46 = 101,
        FIGURE_47 = 102,
        FIGURE_48 = 103,
        FIGURE_49 = 104,
        FIGURE_50 = 105,
        FIGURE_51 = 106,
        FIGURE_52 = 107,
        FIGURE_53 = 108,
        FIGURE_54 = 109,
        FIGURE_55 = 110,
        FIGURE_56 = 111,
        FIGURE_57 = 112,
        FIGURE_58 = 113,
        FIGURE_59 = 114,
        FIGURE_60 = 115,
        FIGURE_61 = 116,
        FIGURE_62 = 117,
        FIGURE_63 = 118,
        FIGURE_64 = 119,
        FIGURE_65 = 120,
        FIGURE_66 = 121,
        FIGURE_67 = 122,
        FIGURE_68 = 123,
        FIGURE_69 = 124,
        FIGURE_70 = 125,
        FIGURE_71 = 126,
        FIGURE_72 = 127,
        FIGURE_73 = 128,
        FIGURE_74 = 129,
        FIGURE_75 = 130,
        FIGURE_76 = 131,
        FIGURE_77 = 132,
        MAX = 133,
    };
}
namespace app::ropeway::gamemastering::SaveDataManager {
    enum class UsableBool : int64_t {
        UNUSABLE = 0,
        TRUE_ = 1,
        FALSE_ = 2,
    };
}
namespace via::wwise {
    enum class User : int64_t {
        User_0 = 0,
        User_1 = 1,
        User_2 = 2,
        User_3 = 3,
        User_None = 4,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class Capture : int64_t {
        SHUTTER_00 = 0,
        BUS_WINDOW_00 = 1,
        BUS_WINDOW_01 = 2,
    };
}
namespace app::ropeway::dynamics::DynamicsDefine {
    enum class EnumState : int64_t {
        STATE_STATIC = 0,
        STATE_DYNAMIC = 1,
        STATE_KEYFRAME = 2,
        STATE_DISABLE = 3,
    };
}
namespace via::gui {
    enum class GridLoopType : int64_t {
        Clamp = 0,
        Loop = 1,
        Next = 2,
    };
}
namespace app::ropeway::PlayerDefine {
    enum class PlayerType : int64_t {
        INVALID = 0xFFFFFFFF,
        PL0000 = 0,
        PL1000 = 1000,
        PL2000 = 2000,
        PL3000 = 3000,
        PL4000 = 4000,
        PL4100 = 4100,
        PL5000 = 5000,
        PL5100 = 5100,
        PL5200 = 5200,
        PL5300 = 5300,
        PL5400 = 5400,
        PL5700 = 5700,
        PL6400 = 6400,
        PL5600 = 5600,
        PL7500 = 7500,
    };
}
namespace via {
    enum class InterpolationFlag : int64_t {
        Unknown = 0,
        Discrete = 1,
        Linear = 2,
        Event = 4,
        Slerp = 8,
        Hermite = 16,
        AutoHermite = 32,
        Bezier = 64,
        AutoBezier = 128,
        OffsetFrame = 256,
        OffsetSec = 512,
        PassEvent = 1024,
        Bezier3D = 2048,
        Range = 4096,
    };
}
namespace app::ropeway::enemy::em7000::Em7000Define {
    enum class ActionStatus : int64_t {
        ACT_IDLE = 0,
        ACT_WALK_W = 1,
        ACT_WALK_G = 2,
        ACT_WAIT_ATTACK = 3,
        ACT_TO_G = 4,
        ACT_TO_W = 5,
        ACT_START_LEON = 6,
        ACT_WAIT_BATTLE = 7,
        ACT_WALK_W_FSM = 8,
        ACT_WALK_G_FSM = 9,
        ACT_BATTLE_START = 10,
        ATK_W_SHORT_R = 2000,
        ATK_W_CONTINUOUS_2 = 2001,
        ATK_W_CONTINUOUS_3 = 2002,
        ATK_W_MIDDLE = 2003,
        ATK_G_SHORT_R = 2004,
        ATK_G_SHORT_F = 2005,
        ATK_G_CONTINUOUS_MIDDLE = 2006,
        ATK_G_GRAPPLE = 2007,
        ATK_W_LARGE_A = 2008,
        ATK_W_CONTINUOUS_A = 2009,
        ATK_W_CONTINUOUS_B = 2010,
        ATK_W_COUTINUOUS_WALK = 2011,
        ATK_G_SHORT_R_FORWARD = 2012,
        ATK_G_SHORT_F_FORWARD = 2013,
        ATK_G_LARGE_A = 2014,
        ATK_G_WALK_ATTACK = 2015,
        ATK_G_PUSH_ATTACK = 2016,
        ATK_W_GRAPPLE = 2017,
        DMG_KNOCKBACK_HEAD = 3000,
        DMG_KNOCKBACK_BODY = 3001,
        DMG_KNOCKBACK_LEG = 3002,
        DMG_KNEEL_DOWN = 3003,
        DMG_ELECTRIC = 3004,
        DMG_FLAME = 3005,
        DMG_ACID = 3006,
        DMG_FLASH = 3007,
        DMG_GRANADE = 3008,
        DIE_NORMAL = 5000,
        DIE_RIGID = 5001,
        DIE_FALL = 5002,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace app::ropeway::gimmick::option::GimmickLadderAIMapEffectorShape {
    enum class PointType : int64_t {
        LOW = 0,
        HIGH = 1,
    };
}
namespace via::wwise::FreeArea {
    enum class FreeArea0to7 : int64_t {
        FreeArea0to7_None = 0xFFFFFFFF,
        FreeArea0to7_0 = 0,
        FreeArea0to7_1 = 1,
        FreeArea0to7_2 = 2,
        FreeArea0to7_3 = 3,
        FreeArea0to7_4 = 4,
        FreeArea0to7_5 = 5,
        FreeArea0to7_6 = 6,
        FreeArea0to7_7 = 7,
    };
}
namespace via::os {
    enum class ClipboardFormat : int64_t {
        TEXT_ANSI = 0,
        TEXT_UNICODE = 1,
        BINARY = 2,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class NaviMeshAttribute : int64_t {
        Ground = 1,
        Wall = 2,
        Ceiling = 4,
        WallCorner = 8,
    };
}
namespace app::ropeway::gamemastering::ActorManager {
    enum class UpdateState : int64_t {
        Idle = 0,
        Request = 1,
        Wait = 2,
        End = 3,
    };
}
namespace app::ropeway::PlayerLightSwitchZone {
    enum class Mode : int64_t {
        DEFAULT = 0,
        ON = 1,
        OFF = 2,
    };
}
namespace app::MathEx {
    enum class RotationOrder : int64_t {
        XYZ = 0,
        XZY = 1,
        YXZ = 2,
        YZX = 3,
        ZXY = 4,
        ZYX = 5,
    };
}
namespace via::hid::camera {
    enum class PlayStationCameraSetConfigResult : int64_t {
        OK = 1,
        ErrorStarted = 2147483649,
        ErrorInvalidType = 2147483650,
        ErrorInternal = 2147487744,
    };
}
namespace app::ropeway::gamemastering::SaveDataManager {
    enum class SaveFileDetailTextList : int64_t {
        DIFFICULTY = 0,
        SAVE_TIMES = 1,
        LOCATION_AREA_ID = 2,
        PURPOSE = 3,
        LANGUAGE = 4,
        PLAYER_NAME = 5,
        COUNT = 6,
    };
}
namespace via::nnfc {
    enum class NpadId : int64_t {
        Pad1 = 0,
        Pad2 = 1,
        Pad3 = 2,
        Pad4 = 3,
        Any = 4,
        Invalid = 5,
    };
}
namespace via::attribute::RemoteFieldAttribute {
    enum class SyncMode : int64_t {
        Copy = 0,
        Kill = 1,
        Transaction = 2,
    };
}
namespace app::ropeway::enemy::em6100::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class SavableDisorderKind : int64_t {
        DOWNED = 0,
        FORBID_STANDUP_BY_STEP = 1,
        BURNUP_BY_ROLE_FINISHED = 2,
        LAYING = 3,
        INTO_INTERACTIVE_GIMMICK = 4,
        LAST_KNIFE_CONST2NECK = 5,
    };
}
namespace app::ropeway::enemy::em6200::MotionPattern {
    enum class DirFB : int64_t {
        Front = 0,
        Back = 1,
    };
}
namespace app::ropeway::gimmick::option::EnemyControlSettings::EnemyControlParam {
    enum class FindPlayerType : int64_t {
        Find = 0,
        Lose = 1,
    };
}
namespace app::ropeway::fsmv2::GroundFixSwitch {
    enum class AdjustMode : int64_t {
        Keep = 0,
        ON_to_OFF = 1,
        OFF_to_ON = 2,
    };
}
namespace via {
    enum class PropertiedEventInvokeTiming : int64_t {
        Equal = 0,
        NotEqual = 1,
        Ever = 2,
        Never = 3,
    };
}
namespace app::ropeway::enemy::tracks::Em7100ColliderTrack {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Press = 0,
        Damage = 1,
        Sensor_Touch = 2,
        Attack_Head = 3,
        Attack = 4,
        Attack_Burst = 5,
        ToPiller_Hold = 6,
        ToPiller_Break = 7,
        Attack_L_Arm = 8,
        Attack_Grapple_Kill = 9,
        MarkerSensor = 10,
        Decal = 11,
        FallDown = 12,
        Camera = 13,
        Attack_Down = 14,
        Press_Aisle = 15,
        Grapple = 16,
        JumpAttack = 17,
        Attack_18 = 18,
        Attack_19 = 19,
        Attack_20 = 20,
        Attack_21 = 21,
        AimTarget = 22,
    };
}
namespace via::userdata {
    enum class VariableError : int64_t {
        None = 0,
        ChangeValueAtWriteProtect = 1,
    };
}
namespace via::hid {
    enum class DeviceIndex : int64_t {
        Index0 = 0,
        Index1 = 1,
        Index2 = 2,
        Index3 = 3,
        Index4 = 4,
        Index5 = 5,
        Index6 = 6,
        Index7 = 7,
        Index8 = 8,
        Index9 = 9,
        Index10 = 10,
        Index11 = 11,
        Index12 = 12,
        Index13 = 13,
        Index14 = 14,
        Index15 = 15,
        Index16 = 16,
        Index17 = 17,
        Index18 = 18,
        Index19 = 19,
        Index20 = 20,
        Index21 = 21,
        Index22 = 22,
        Index23 = 23,
        Index24 = 24,
        Index25 = 25,
        Index26 = 26,
        Index27 = 27,
        Index28 = 28,
        Index29 = 29,
        Index30 = 30,
        Index31 = 31,
        Index32 = 32,
        Index33 = 33,
        Index34 = 34,
        Index35 = 35,
        Index36 = 36,
        Index37 = 37,
        Index38 = 38,
        Index39 = 39,
        Index40 = 40,
        Index41 = 41,
        Index42 = 42,
        Index43 = 43,
        Index44 = 44,
        Index45 = 45,
        Index46 = 46,
        Index47 = 47,
        All = 65535,
        Invalid = 65536,
        Max = 65537,
    };
}
namespace app::ropeway::effect::script::Ti_DamageHitType {
    enum class DamageHitTypes : int64_t {
        None = 0,
        Normal = 1,
    };
}
namespace via::storage {
    enum class ChunkInstalledDevice : int64_t {
        None = 0,
        Slow = 1,
        Fast = 2,
    };
}
namespace app::ropeway::enemy::tracks::Em7000ColliderTrack {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Press = 0,
        Sensor_Touch = 1,
        Attack_BODY_M = 2,
        Attack_BURST = 3,
        Attack_PROUD = 4,
        Attack_Grapple = 5,
        Decal = 6,
        Camera = 7,
        Attack_PUSH = 8,
        Attack_Dummy01 = 9,
        Attack_Dummy02 = 10,
        Attack_Dummy03 = 11,
        Attack_Dummy04 = 12,
        MarkerSensor = 13,
    };
}
namespace via::effect::script::ObjectEffectManager::ExternalProperty {
    enum class PlayState : int64_t {
        Idle = 0,
        Play = 1,
        Finish = 2,
        Kill = 3,
    };
}
namespace app::ropeway::EquipmentDefine {
    enum class MeleeAttack : int64_t {
        Invalid = 0xFFFFFFFF,
        High = 0,
        Low = 1,
        TerraIN_ = 2,
        Fatal = 3,
        Defense = 4,
        Stealth = 5,
    };
}
namespace via::navigation::PathObject {
    enum class PortalEndEdgeAttrType : int64_t {
        P0 = 0,
        P1 = 1,
        TypeNum = 2,
    };
}
namespace via::os::dialog {
    enum class State : int64_t {
        None = 0,
        Busy = 1,
    };
}
namespace app::ropeway::fsmv2::enemy::em7000 {
    enum class RANK_MOTION_SPEED : int64_t {
        WMODE_WALK = 1012468819,
        WMODE_CATCH_WALK = 648986048,
        WMODE_RUN = 879001903,
        GMODE_WALK = 261095494,
        GMODE_ANGER_WALK = 3020054378,
        GMODE_RUN = 9641753,
    };
}
namespace app::ropeway::enemy::em7000::fsmv2::action::MotionCameraPlayRequest {
    enum class CameraTarget : int64_t {
        CurrentPlayer = 0,
        Owner = 1,
    };
}
namespace app::ropeway::gamemastering::GameMaster {
    enum class MachineType : int64_t {
        INVALID = 0,
        PC = 1,
        PS4 = 2,
        XB1 = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickAction {
    enum class SuspendTypeBase : int64_t {
        DEVELOP = 0,
        SCENARIO_RESTRICT = 1,
        _INHERIT = 2,
    };
}
namespace app::ropeway::enemy::em5000::fsmv2::action::Em5000FsmAction_SetHang {
    enum class WeakPartsSettings : int64_t {
        HIDE = 0,
        VISIBLE = 1,
    };
}
namespace via::render {
    enum class ColorWrite : int64_t {
        EnableRed = 1,
        EnableGreen = 2,
        EnableBlue = 4,
        EnableAlpha = 8,
        EnableRGB = 7,
        EnableAll = 15,
    };
}
namespace app::ropeway::timeline::TimelineActorController {
    enum class ObjectType : int64_t {
        Survivor = 0,
        EnemyBase = 1,
        Gimmick = 2,
        SetModel = 3,
        Camera = 4,
        Undefined = 5,
    };
}
namespace app::ropeway::Em6000Think {
    enum class STATE : int64_t {
        SightChecked = 0,
        ScreenTaregt = 1,
        Swim = 2,
        Vomit = 3,
        ToWalkContinue = 4,
        InWater = 5,
        WishToSwimHold = 6,
        DesireToSwimHold = 7,
        NoGunSensor = 8,
    };
}
namespace System::Text {
    enum class NormalizationForm : int64_t {
        FormC = 1,
        FormD = 2,
        FormKC = 5,
        FormKD = 6,
    };
}
namespace via::clr {
    enum class BindingFlags : int64_t {
        Default = 0,
        IgnoreCase = 1,
        DeclaredOnly = 2,
        Instance = 4,
        Static = 8,
        Public = 16,
        NonPublic = 32,
        FlattenHierarchy = 64,
        InvokeMethod = 256,
        CreateInstance = 512,
        GetField = 1024,
        SetField = 2048,
        GetProperty = 4096,
        SetProperty = 8192,
        PutDispProperty = 16384,
        PutRefDispProperty = 32768,
        ExactBinding = 65536,
        SuppressChangeType = 131072,
        OptionalParamBinding = 262144,
        IgnoreReturn = 16777216,
    };
}
namespace via::hid::VrTracker {
    enum class CalibrationType : int64_t {
        Position = 0,
        All = 1,
    };
}
namespace via::os {
    enum class FileSeek : int64_t {
        BegIN_ = 0,
        Current = 1,
        End = 2,
    };
}
namespace via::gui {
    enum class BarDirection : int64_t {
        Horizontal = 0,
        Vertical = 1,
        Max = 2,
    };
}
namespace via::motion::ChainResource {
    enum class DirectionOfEmission : int64_t {
        Global = 0,
        Local = 1,
        GroupLocal = 2,
    };
}
namespace app::ropeway::ParamCurveAnimator::ProcessPack {
    enum class AnimTargetType : int64_t {
        NONE = 0,
        POS = 1,
        ROT = 2,
        EMISSIVE = 3,
        MAT_VARIABLE = 4,
        LIGHT = 5,
        EFFECT = 6,
    };
}
namespace app::ropeway::enemy::em5000::RoleAction {
    enum class RolePattern : int64_t {
        IDLE = 0,
        HANG = 1,
        DEAD_IDLE = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class Em0000DirtyPreset_Face : int64_t {
        Face00 = 0,
        Face01 = 1,
        Face02 = 2,
        Face03 = 3,
        Face04 = 4,
        Face05 = 5,
    };
}
namespace app::ropeway::enemy::savedata::EmSaveData {
    enum class FlagDataBitIndex : int64_t {
        RELOCATION_MODE = 0,
        NO_LOITERING_ON_RELOCATION = 1,
        ROLE_MODE = 2,
        FIRST_AVAILABLE_POS_SAVE = 3,
        ELIMINATED = 4,
    };
}
namespace via::physics {
    enum class GateType : int64_t {
        Both = 0,
        EnterA = 1,
        EnterB = 2,
    };
}
namespace app::ropeway::gui::FloorMapSt44Behavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_4_650_0 = 1,
    };
}
namespace via::render::RoughTransparent {
    enum class DebugMode : int64_t {
        None = 0,
        Downsample = 1,
        Blur = 2,
    };
}
namespace via::motion::detail::MotionFsm2TransitionData {
    enum class StartType : int64_t {
        Frame = 0,
        NormalizedTime = 1,
        SyncTime = 2,
        AutoSyncTime = 3,
        AutoSyncTimeSamePointCount = 4,
    };
}
namespace app::ropeway::enemy::em5000::WeakPartsStatus {
    enum class STATE : int64_t {
        ENABLE = 0,
        DISABLE = 1,
        RESURRECTION = 2,
    };
}
namespace app::ropeway::GimmickAttackCategorySettings {
    enum class AttackCategorySetting : int64_t {
        NONE = 0,
        BREAKABLE_PILLAR = 1,
    };
}
namespace via::render::layer::CubemapCapture {
    enum class BAKEOPTION : int64_t {
        NONE = 0,
        BOUNCE_PROBE_ENABLE = 1,
        FULL_ILLUMINATION_ENABLE = 2,
    };
}
namespace via::render {
    enum class GraphicsFeatures : int64_t {
        None = 0,
        HMD = 1,
        InterlacedRendering = 2,
        ESRAM = 4,
        AsyncCompute = 8,
        HDRCurvePQ = 16,
        FastAsyncCompute = 32,
        CapableHDRCurvePQ = 64,
        EQAACheckerBoardRendering = 128,
        HardwareCBRIDBuffer = 256,
        RequireCBRIDBuffer = 512,
        LateCBRResolve = 1024,
        UseIDBuffer = 768,
        UseCheckerBoard = 2048,
        FastCBRSetting = 1152,
        DefaultCBRSetting = 128,
        HighQualityCBRSetting = 640,
        PS4ProtCBRSetting = 1408,
        NotSupportedPredicateRenderingOnBuffer = 4096,
        UavOverlapSupported = 8,
        VendorAMD = 16777216,
        VendorNVIDIA = 33554432,
        VendorINTEL = 67108864,
        VendorUnkown = 0,
        Gen2 = 268435456,
        Gen3 = 536870912,
        Gen4 = 1073741824,
        GL_Family = 134217728,
    };
}
namespace via::areamap::AreaConnectionObject {
    enum class ConnectionState : int64_t {
        Open = 0,
        Locked = 1,
        Closed = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickSteamPipe {
    enum class AttackParam : int64_t {
        TO_EM = 0,
        TO_PL = 1,
        TO_EM_CRITICAL = 2,
        TO_EM_NOTICE = 3,
        MAX = 4,
    };
}
namespace via::behaviortree::Action {
    enum class MethodImpls : int64_t {
        AppendedUpdateStart = 1,
        AppendedUpdate = 2,
        Start = 4,
        Update = 8,
        End = 16,
        OnNodeEndNotified = 32,
    };
}
namespace app::ropeway::fsmv2::enemy::condition::Em7200FsmCondition_WalkStartDirection {
    enum class TYPE : int64_t {
        Left90 = 0,
        Left180 = 1,
    };
}
namespace via::render::SoftBloom {
    enum class Algorithm : int64_t {
        Standard = 0,
        StandardV2 = 1,
        StandardV3 = 2,
        CoDAW = 3,
    };
}
namespace via::render {
    enum class StencilValue : int64_t {
        Zero = 0,
        All = 255,
        SceneBeauty = 1,
        SceneSubsurface = 2,
        SceneDecalRecive = 4,
        MaskIgnoreDepthMesh = 8,
    };
}
namespace via::render {
    enum class OptionFlag : int64_t {
        None = 0,
        TextureCube = 4,
        DrawIndirectArgs = 16,
        BufferAllowRawViews = 32,
        BufferStructured = 64,
        DX11Mask = 255,
        HTile = 256,
        HTileResolveToShadowMmeory = 512,
        DisableWithoutDecompress = 1024,
        CPUReadableResource = 2048,
        LinearLayout = 4096,
        NoZeroFill = 8192,
        DeltaColorCompression = 16384,
        NoCompression = 32768,
        MemoryBankRotation1 = 65536,
        MemoryBankRotation2 = 131072,
        MemoryBankRotation3 = 262144,
        MemoryBankRotationMask = 458752,
        ZCullStoreBuffer = 1048576,
        AdaptiveZCull = 2097152,
    };
}
namespace via::gui {
    enum class ResolutionAdjustScale : int64_t {
        None = 0,
        Stretch = 1,
        FitSmallRatioAxis = 2,
        FitLargeRatioAxis = 3,
        Max = 4,
    };
}
namespace app::ropeway::gimmick::option::OuterDynamicMotionBankSettings::Info {
    enum class SetKind : int64_t {
        Add = 0,
        Remove = 1,
    };
}
namespace app::ropeway::gui::DialogBehavior {
    enum class Priority : int64_t {
        LOW = -1000,
        DEFAULT = 0,
        DEVICE_PAIRING = 80,
        HIGH = 1000,
    };
}
namespace app::ropeway::enemy::em6200::Em6200Think {
    enum class NaviDirection : int64_t {
        Front = 0,
        Left = 1,
        Right = 2,
    };
}
namespace via::AnimationCurveData3D {
    enum class Wrap : int64_t {
        Once = 0,
        Loop = 1,
        Loop_Always = 2,
    };
}
namespace via {
    enum class SharePlayLevel : int64_t {
        Full = 0,
        ScreenOnly = 1,
        None = 2,
    };
}
namespace app::ropeway::gimmick::option::GimmickCameraSettings {
    enum class CameraType : int64_t {
        FIX = 0,
        MOTION = 1,
        TWIRL = 2,
    };
}
namespace via::wwise {
    enum class EvaluatesEstimatedDurationType : int64_t {
        None = 0,
        Minimum = 1,
        Maximum = 2,
    };
}
namespace via::render::layer {
    enum class ForwardSolidSegment : int64_t {
        ZIgnorePrepassSolid = 0,
        ZIgnorePrepassTwoSide = 1,
        ZIgnorePrepassTwoSideAlphaTest = 2,
        ZIgnorePrepassAlphaTest = 3,
        ZPrepassSolid = 4,
        ZPrepassTwoSide = 5,
        ZPrepassTwoSideAlphaTest = 6,
        ZPrepassAlphaTest = 7,
        ZPrepassVfx = 8,
        Solid = 9,
        TwoSide = 10,
        VelocitySolid = 11,
        VelocityTwoSide = 12,
        EmissiveSolid = 13,
        EmissiveTwoSide = 14,
        MeshDecal = 15,
        AfterDepthConfirmed = 16,
        PostFill = 17,
    };
}
namespace via::motion::script::FootEffectController {
    enum class JointSideType : int64_t {
        Left = 0,
        Right = 1,
        Other = 2,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::EmCommonMFsmAction_ResetWince {
    enum class Type : int64_t {
        START = 0,
        END = 1,
    };
}
namespace app::ropeway::supportitem::SupportItemAttachmentController {
    enum class EventClass : int64_t {
        Action = 0,
        Effect = 1,
        Invalid = 0xFFFFFFFF,
    };
}
namespace app::ropeway::gamemastering::GameMaster {
    enum class OSType : int64_t {
        INVALID = 0,
        WINDOWS = 1,
        PS4 = 2,
    };
}
namespace via::effect::detail {
    enum class DistortionType : int64_t {
        Blur = 0,
        Refract = 1,
        BlurTexture = 2,
    };
}
namespace via::render::command::MultiDrawIndexedInstancedIndirectCount {
    enum class HINT : int64_t {
        NONE = 0,
        PREDICATE = 1,
    };
}
namespace via::effect::script::EPVDataBase {
    enum class EffectRelationType : int64_t {
        FollowParent = 0,
        InitializationParent = 1,
        World = 2,
        WorldAng = 3,
        FollowCamera = 4,
        FollowCameraPos = 5,
        CameraNodeBillboard = 6,
    };
}
namespace app::ropeway::enemy::em3000::Em3000Think {
    enum class WwiseFlag : int64_t {
        TRUE_ = 0,
        FALSE_ = 1,
    };
}
namespace via::render::LightVolumeDecal {
    enum class ValidFlag : int64_t {
        None = 0,
        Color = 1,
        Alpha = 2,
        BaseAlpha = 3,
        Normal = 4,
        Roughness = 8,
        NormalRoughness = 12,
        All = 15,
    };
}
namespace app::ropeway::enemy::gimmick::window {
    enum class WindowActionType : int64_t {
        KNOCK = 0,
        BREAK = 1,
        IN_ = 2,
        BREAKIN_ = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickBlastBoiler {
    enum class AttackParam : int64_t {
        TO_EM_STEAM = 0,
        TO_EM_BLAST = 1,
        TO_PL_STEAM = 2,
        TO_PL_BLAST = 3,
        MAX = 4,
    };
}
namespace via::dynamics::DefinitionResource {
    enum class ChannelAttribute : int64_t {
        InitialValue = 0,
        Max = 1,
    };
}
namespace app::ropeway::PrefabInfo {
    enum class LoadStatus : int64_t {
        Unload = 0,
        Requested = 1,
        Loading = 2,
        Load = 3,
        Unloading = 4,
    };
}
namespace app::ropeway::gimmick::action::AreaAccessSensor {
    enum class PlAttrType : int64_t {
        HEAT = 0,
    };
}
namespace app::ropeway::gamemastering::MovieManager {
    enum class PauseRequestState : int64_t {
        Idle = 0,
        Pause = 1,
        Resume = 2,
        Skip = 3,
    };
}
namespace via::effect::detail {
    enum class ExternType : int64_t {
        Float = 0,
        Color = 1,
        LimitedFloat = 2,
        Bool = 3,
    };
}
namespace app::ropeway::survivor::npc::moveline {
    enum class LineTypeKind : int64_t {
        Straight = 0,
        Bezier = 1,
    };
}
namespace app::ropeway::gui::GimmickSafeBoxDialBehavior {
    enum class MoveType : int64_t {
        TypeA = 0,
        TypeB = 1,
    };
}
namespace app::ropeway::enemy::em7000::Em7000Define {
    enum class GMode : int64_t {
        GMode_William = 0,
        GMode_G1 = 1,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::EmCommonMFsmAction_GM_Interact {
    enum class DoorInteractActionType : int64_t {
        KNOCK = 0,
        OPEN = 1,
    };
}
namespace app::ropeway::enemy::common::fsmv2::condition::LastDamageDirection {
    enum class SideDir : int64_t {
        Left = 0,
        Right = 1,
    };
}
namespace via::motion::IkLookAt {
    enum class DistanceState : int64_t {
        None = 0,
        Follow = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickWiringNodeBase {
    enum class MotorShapeType : int64_t {
        FAN = 0,
        INCINERATOR = 1,
    };
}
namespace app::ropeway::weapon::shell::ShellCartridgeController {
    enum class STATE : int64_t {
        WaitInst = 0,
        Ready = 1,
        Request = 2,
        Generate = 3,
        Wait = 4,
        Invalid = 5,
    };
}
namespace app::ropeway::gui::ItemPortableSafeBehavior {
    enum class StateEnum : int64_t {
        DecideOff = 0,
        DecideOn = 1,
        DecideOnOut = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickDoorBase {
    enum class AttackType : int64_t {
        BURST_OPEN_TO_EM = 0,
        BURST_OPEN_TO_PL = 1,
        MAX = 2,
    };
}
namespace app::ropeway::enemy::EnemyWeaponDataUserData::WeaponValueInfoBase {
    enum class RocketLauncherShellType : int64_t {
        Bullet = 0,
        Explosion = 1,
        Blast = 2,
        ShockWave = 3,
    };
}
namespace app::ropeway::gimmick::action::TriggerKeyCustomizer {
    enum class LStickType : int64_t {
        ZP = 0,
        ZM = 1,
        XP = 2,
        XM = 3,
    };
}
namespace via::render {
    enum class HMDResolutionType : int64_t {
        None = 0,
        FillAfterLighting = 1,
        FillAfterTransparent = 2,
    };
}
namespace System {
    enum class ConsoleColor : int64_t {
        Black = 0,
        DarkBlue = 1,
        DarkGreen = 2,
        DarkCyan = 3,
        DarkRed = 4,
        DarkMagenta = 5,
        DarkYellow = 6,
        Gray = 7,
        DarkGray = 8,
        Blue = 9,
        Green = 10,
        Cyan = 11,
        Red = 12,
        Magenta = 13,
        Yellow = 14,
        White = 15,
    };
}
namespace app::ropeway::gimmick::option::SurvivorJackActionControlSettings::Param {
    enum class OrderKind : int64_t {
        Execute = 0,
        StopWaitStart = 1,
        StopLoop = 2,
        StopSecondLoop = 3,
        Terminate = 4,
        ForceTerminate = 5,
        Invalid = 6,
    };
}
namespace via::clr {
    enum class MethodExceptionFlag : int64_t {
        MethoExceptionFlag_Exception = 0,
        MethoExceptionFlag_Filter = 1,
        MethoExceptionFlag_Finaly = 2,
        MethoExceptionFlag_Fault = 3,
    };
}
namespace app::ropeway::fsmv2::condition::TransitionFrame {
    enum class FrameReferenceType : int64_t {
        HEAD = 0,
        TAIL = 1,
    };
}
namespace app::ropeway::MultiBoneIK {
    enum class SolverType : int64_t {
        CyclicCoordinateDescent = 0,
    };
}
namespace app::ropeway::ParentMotionSynchronizer {
    enum class PlayMode : int64_t {
        Synchronize = 0,
        Overwrite = 1,
    };
}
namespace app::ropeway::enemy::em6000 {
    enum class PartsID : int64_t {
        Body = 0,
        Orvid = 3,
        OrvidBase = 4,
        Eye = 6,
        Coating = 7,
        BrokenEye = 8,
        BrokenCoating = 9,
        PusID_Top = 10,
        PusID_Last = 130,
        PusID_Offset = 10,
    };
}
namespace via::render {
    enum class LightPowerUnitType : int64_t {
        Lumen = 0,
        Candela = 1,
    };
}
namespace System {
    enum class DayOfWeek : int64_t {
        Sunday = 0,
        Monday = 1,
        Tuesday = 2,
        Wednesday = 3,
        Thursday = 4,
        Friday = 5,
        Saturday = 6,
    };
}
namespace app::ropeway::gimmick::action::AreaAccessSensor {
    enum class ProcessType : int64_t {
        FLAG = 0,
        PL_ATTR = 1,
    };
}
namespace app::ropeway::gui::TitleLBehavior {
    enum class MenuListType : int64_t {
        LOST_ONLY = 0,
        LOST_4TH = 1,
        ALL_OPEN = 2,
        INVALID = 3,
    };
}
namespace app::ropeway::gamemastering::RecordManager {
    enum class RecordType : int64_t {
        ALL = 0,
        SCENARIO = 1,
        SURVIVOR = 2,
        HIDDEN = 3,
    };
}
namespace app::ropeway::gimmick::action::RestageEventFinished {
    enum class Action : int64_t {
        DISP_WHEN_FINISHED = 0,
        HIDE_WHEN_FINISHED = 1,
        MOTION_BEFORE_AND_AFTER = 2,
        PARTS_BEFORE_AND_AFTER = 3,
        MATERIAL_F_BEFORE_AND_AFTER = 4,
    };
}
namespace via::hid::camera {
    enum class PlayStationCameraCaptureMemoryType : int64_t {
        Onion = 1,
        Garlic = 2,
    };
}
namespace via::movie::Movie {
    enum class LoadResourceState : int64_t {
        StartProcessing = 0,
        WaitingForResourceValidation = 1,
        AcceptedCreatePlayer = 2,
        AcceptedInitializeFileBuffer = 3,
        AcceptedWaitForLibraryInitialization = 4,
        ErrorResourceValidation = 5,
        ErrorLibraryStartup = 6,
    };
}
namespace app::ropeway::GimmickOptionDoorSettings::DoorSetParam::Unlock {
    enum class KeyType : int64_t {
        NONE = 0,
        ITEM = 1,
        INTERACT = 2,
    };
}
namespace via::hid {
    enum class NpadHandheldActivationMode : int64_t {
        Dual = 0,
        Single = 1,
        None = 2,
    };
}
namespace app::ropeway::enemy::em6300::MotionPattern {
    enum class WalkStart : int64_t {
        Front = 0,
        FrontL = 1,
        FrontR = 2,
        BackL = 3,
        BackR = 4,
    };
}
namespace app::ropeway::gamemastering::RogueDevelopManager {
    enum class State : int64_t {
        INVALID = 0xFFFFFFFF,
        INITIALIZE = 0,
        WAIT_TO_INGAME = 1,
        INITIALIZE_TO_INGAME = 2,
        ENABLE = 3,
    };
}
namespace app::ropeway::SurvivorMotionKind {
    enum class StatusType : int64_t {
        Fine = 0,
        Caution = 1,
        Danger = 2,
        Hold = 3,
    };
}
namespace app::ropeway::enemy::em4000::Em4000Gimmick {
    enum class GimmickType : int64_t {
        JUMP = 0,
        HOLE = 1,
        BARRICADE = 2,
        ZONE_LIMIT = 3,
        FENCE_CLIMB = 4,
        CAR_CLIMB = 5,
    };
}
namespace via::eq::UnaryOperator {
    enum class Type : int64_t {
        MIN_ = 0,
    };
}
namespace via::gui {
    enum class MaskMode : int64_t {
        Keep = 0,
        Default = 1,
        Reverse = 2,
        Disable = 3,
        ApplyToParent = 4,
    };
}
namespace via::network::AutoMatchmaking {
    enum class MatchPhase : int64_t {
        Phase_None = 0,
        Phase_Init = 1,
        Phase_CreateSession = 2,
        Phase_CreateWait = 3,
        Phase_SetSearchRule = 4,
        Phase_SearchReady = 5,
        Phase_SearchSession = 6,
        Phase_SearchWait = 7,
        Phase_ThinkRule = 8,
        Phase_GiveupSession = 9,
        Phase_GiveupWait = 10,
        Phase_RebootSession = 11,
        Phase_JoinSession = 12,
        Phase_JoinWait = 13,
        Phase_SyncAttribute = 14,
        Phase_AcceptWait = 15,
        Phase_InGame = 16,
        Phase_SearchInterval = 17,
        Phase_Timeout = 18,
        Phase_TimeoutWait = 19,
        Phase_Fatal = 20,
        Phase_XboxInit = 21,
        Phase_XboxInitWait = 22,
        Phase_XboxMatchWait = 23,
        Phase_XboxMatchWait2 = 24,
    };
}
namespace via::motion::detail::IkNboneSolver {
    enum class weightType : int64_t {
        Basic = 0,
        Top = 1,
        Bottom = 2,
        Medium = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickSparkGimmickControl {
    enum class SparkState : int64_t {
        STANDBY = 0,
        SPARK = 1,
        FINISH = 2,
        MAX = 3,
    };
}
namespace via::behaviortree::TreeNode {
    enum class WorkStatus : int64_t {
        Off = 0,
        Start = 1,
        Work = 2,
        Ignore = 3,
    };
}
namespace app::ropeway::navigation::Definition {
    enum class Attr0 : int64_t {
        Ground = 1,
        Wall = 2,
        Ceiling = 4,
        WallCorner = 8,
        SewerSideway = 16,
    };
}
namespace app::ropeway::EnemyMoveController::ForceMoveInfo {
    enum class EndTypeAttr : int64_t {
        Attention = 2,
        Find = 4,
        TimeOut = 8,
        UserEvent = 16,
    };
}
namespace via::navigation::AIMapEffector {
    enum class EdgePrecisionType : int64_t {
        x1 = 0,
        x10 = 1,
        x100 = 2,
    };
}
namespace app::ropeway::enemy::common::behaviortree::condition::CheckHp {
    enum class Expression : int64_t {
        EQ = 0,
        NE = 1,
        GT = 2,
        GE = 3,
        LT = 4,
        LE = 5,
    };
}
namespace app::ropeway::PlayerDefine {
    enum class ForceChangeState : int64_t {
        INVALID = 0,
        IDLE = 1,
        WALK = 2,
        JACK = 3,
        EVENT = 4,
        REACTION = 5,
        EXTERNAL_OVERWRITE_START = 6,
        EXTERNAL_OVERWRITE_END = 7,
        SAME = 8,
    };
}
namespace via::ThreadPool::JobManager::Job {
    enum class State : int64_t {
        Uninitialized = 0,
        Started = 1,
        Wait = 2,
        WaitRelease = 3,
        Ended = 4,
    };
}
namespace app::ropeway::gimmick::option::GimmickOptionEnemyTrapSettings {
    enum class AreaColShape_Enum : int64_t {
        BOX = 0,
        SPHERE = 1,
    };
}
namespace via::motion {
    enum class InterpolationMode : int64_t {
        None = 0,
        FrontFade = 1,
        CrossFade = 2,
        SyncCrossFade = 3,
        SyncPointCrossFade = 4,
    };
}
namespace app::ropeway::weapon::SparkShotLCDController {
    enum class ChargeType : int64_t {
        ImcompleteCharge = 0,
        MaxCharge = 1,
    };
}
namespace app::ropeway::gui::RogueInventoryDetailBehavior {
    enum class PuzzleModeEnum : int64_t {
        None = 0,
        PortableSafe = 1,
        CarKey = 2,
        Oscilloscope = 3,
    };
}
namespace app::ropeway::system::RingBufferController {
    enum class StateList : int64_t {
        Invalid = 0,
        Initialize = 1,
        Standby = 2,
        Ready = 3,
        Release = 4,
        Released = 5,
    };
}
namespace via::render::Bloodshed {
    enum class SourceUV : int64_t {
        Primary = 0,
        Secondary = 1,
    };
}
namespace app::ropeway::survivor::npc::NpcOrderDefine {
    enum class ActionCommandKind : int64_t {
        InsteadAction = 0,
        SetMotionSpeedRate = 1,
        StartRainArea = 2,
        EndRainArea = 3,
        Invalid = 4,
    };
}
namespace app::ropeway::timeline::TimelineEventSettingTable::FlowSetting::SkipParam {
    enum class SkipType : int64_t {
        EndFrame = 0,
        Manual = 1,
    };
}
namespace app::ropeway::enemy::em7100::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_RUN = 2,
        ACT_WAIT_ATTACK = 3,
        ACT_CONTINUE_REACTION = 4,
        ACT_DEMO_00 = 5,
        DMG_TIRED = 6,
        ACT_WAIT_DEMO = 7,
        ATK_HIGH_RQ = 2000,
        ATK_HIGH_R = 2001,
        ATK_LOW_L = 2002,
        ATK_LOW_R = 2003,
        ATK_COMBO_2 = 2004,
        ATK_COMBO_3 = 2005,
        ATK_TURN_L = 2006,
        ATK_TURN_R = 2007,
        ATK_ZERO = 2008,
        ATK_VIRTICAL = 2009,
        ATK_COMBO_5 = 2010,
        ATK_COMBO_8 = 2011,
        ATK_JUMP = 2012,
        ATK_LONG_MURDER = 2013,
        ATK_LONG_MURDER_R90 = 2014,
        ATK_LONG_MURDER_COMBO = 2015,
        ATK_GRAPPLE_KILL = 2016,
        ATK_VIRTICAL_TYPE2 = 2017,
        ATK_LOW_R_HEAVY = 2018,
        ATK_HIGH_R_HEAVY = 2019,
        ATK_VIRTICAL_HEAVY = 2020,
        ATK_DOWN_ATTACK = 2021,
        ATK_GRAPPLE = 2022,
        ATK_DOWN_ATTACK_V2 = 2023,
        DMG_KNOCKBACK_HEAD_F = 3000,
        DMG_KNOCKBACK_BODY_F = 3001,
        DMG_KNOCKBACK_BODY_B = 3002,
        DMG_ELECTRICAL = 3003,
        DMG_FLASH = 3004,
        DMG_SHOULDER_EYE = 3005,
        DMG_BACK_EYE = 3006,
        DMG_CLING = 3007,
        DIE_NORMAL = 5000,
        DIE_TRANSFORM = 5001,
        DIE_RIDGID = 5002,
        DIE_CLING = 5003,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace via::gui::detail {
    enum class RectVertex : int64_t {
        LeftTop = 0,
        LeftBottom = 1,
        RightTop = 2,
        RightBottom = 3,
        Max = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickDelegateCollisionProcedure {
    enum class ConditionType : int64_t {
        NONE = 0,
        COLLISION_TYPE_MATCH = 1,
        MAX = 2,
    };
}
namespace app::ropeway::network::service::RichpresenceServiceController {
    enum class ProcessType : int64_t {
        Idle = 0,
        Normal = 1,
        Request = 2,
        Requested = 3,
        ContextWait = 4,
    };
}
namespace via::effect::gpgpu::detail {
    enum class FieldType : int64_t {
        None = 0,
        Vector = 1,
        Volume = 2,
    };
}
namespace via::motion::tree::CompareNode {
    enum class Operation : int64_t {
        Eq = 0,
        Neq = 1,
        Ge = 2,
        Le = 3,
        Gt = 4,
        Lt = 5,
    };
}
namespace app::ropeway::network::service::RichpresenceServiceControllerForXB1 {
    enum class SubProcessType : int64_t {
        Waiting = 0,
        Request = 1,
        RequestWaiting = 2,
    };
}
namespace via {
    enum class SystemServiceAccountPickerResult : int64_t {
        Success = 0,
        Cancel = 1,
        Failed = 2,
    };
}
namespace app::ropeway::enemy::em6200::Em6200Think {
    enum class ENABLE_ACTION : int64_t {
        Anythig = 0,
        Hook = 1,
        Punch = 2,
        Grapple = 3,
        SidePunch = 4,
        TurnBlow = 5,
        OtherGrappleWait = 6,
        LadderWait = 7,
    };
}
namespace app::ropeway::fsmv2::U32VariableAction {
    enum class DataType : int64_t {
        USERDATA = 0,
        VARIABLES = 1,
    };
}
namespace via::navigation {
    enum class WarningType : int64_t {
        StartNodeNotFound = 0,
        EndNodeNotFound = 1,
        PathNotFound = 2,
        PathfindInterrupt = 3,
        InvalidHybridPath = 4,
        WarningTypeNum = 5,
    };
}
namespace app::ropeway::ParamCurveAnimator::GroupData {
    enum class RoutineType : int64_t {
        WAIT = 0,
        START = 1,
        LOOP = 2,
    };
}
namespace app::ropeway::VariableHubAccessor {
    enum class IndexType : int64_t {
        Name = 0,
        Hash = 1,
        GUID = 2,
    };
}
namespace app::ropeway::enemy::em6300::MotionPattern {
    enum class DirFB : int64_t {
        Front = 0,
        Back = 1,
    };
}
namespace via::gui::MaterialBinding {
    enum class BindingError : int64_t {
        None = 0,
        InvalidMaterial = 1,
        InvalidClusterName = 2,
        InvalidParamName = 4,
        InvalidType = 8,
    };
}
namespace app::ropeway::gui::RogueInventoryBehavior {
    enum class Mode : int64_t {
        Initialize = 0,
        CallOpen = 1,
        CallOpenGimmick = 2,
        EnterInventory = 3,
        CallClose = 4,
        LeaveInventory = 5,
        CallOpenDetail = 6,
        FinishDetail = 7,
        CallPickUpFirst = 8,
        CallPickUpFirstReady = 9,
        FinishPickUpFirst = 10,
    };
}
namespace app::ropeway::gimmick::action::GimmickElevator {
    enum class Routine : int64_t {
        WAIT = 0,
        CLOSE = 1,
        OPEN = 2,
        SHIFT = 3,
    };
}
namespace app::ropeway::gui::FloatIconParam {
    enum class ManualPositionCoord : int64_t {
        LOCAL = 0,
        WORLD = 1,
        JOINT = 2,
    };
}
namespace via::ffts {
    enum class ErrType : int64_t {
        None = 0,
        Critical = 0xFFFFFFFF,
        NoMemory = -2,
        Forceword = 2147483647,
    };
}
namespace app::ropeway::enemy::sensor::EnemySensor {
    enum class SensorKind : int64_t {
        SIGHT = 1,
        HEARING = 2,
        TOUCH = 4,
    };
}
namespace app::Collision::CollisionSystem::HitResult {
    enum class PlaneType : int64_t {
        NONE = 0,
        GROUND = 1,
        SLOPE = 2,
        WALL = 3,
        CEILING = 4,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class ScreenPhase : int64_t {
        FASTEST = -91,
        FadeManager = -90,
        FadeController = -89,
        Loading = -88,
        Pause = -87,
        LATEST = -86,
    };
}
namespace via::render::AlphaAA {
    enum class AntialiasingMethod : int64_t {
        FXAA = 0,
        SMAA1x = 1,
        SMAAT2x = 2,
    };
}
namespace app::ropeway::camera::fsmv2::action::ActionCameraPlayRequestRoot {
    enum class MotSeqCheckTargetMask : int64_t {
        FsmOwner = 1,
        MotionJackTarget = 2,
    };
}
namespace app::ropeway::RagdollController {
    enum class RequestTriggerKind : int64_t {
        TransformRagdoll = 0,
        ReturnRagdoll = 1,
        ResetTransform = 2,
        AddImpulse = 3,
        ClearMotion = 4,
        UpdatePoseStatus = 5,
        DynamicStatePick = 6,
        BackToSafePosition = 7,
    };
}
namespace app::ropeway::OptionManager {
    enum class GraphicsPresetType : int64_t {
        Default = 0,
        Low = 1,
        Medium = 2,
        High = 3,
        Highest = 4,
    };
}
namespace via::os {
    enum class FileOptionalAttr : int64_t {
        Create = 1,
        Append = 2,
        Trunc = 4,
        Sync = 8,
    };
}
namespace via::effect::detail {
    enum class LinePreset : int64_t {
        Follow = 0,
        Length = 1,
        ChaIN_ = 2,
    };
}
namespace app::ropeway::DetectionParam {
    enum class Level : int64_t {
        Lv0 = 0,
        Lv1 = 1,
        Lv2 = 2,
        Lv3 = 3,
        Lv4 = 4,
        Lv5 = 5,
        Max = 6,
    };
}
namespace app::ropeway::FadeManager {
    enum class FadeType : int64_t {
        INGAME = 0,
        MENU = 1,
        LOADING = 2,
        FSM = 3,
        EVENT = 4,
        SKIP = 5,
        INTERLUDE = 6,
        NUM = 7,
    };
}
namespace via::render::SSRControl {
    enum class SSRResolvePointNum : int64_t {
        SSRResolvePointNum_1 = 0,
        SSRResolvePointNum_2 = 1,
        SSRResolvePointNum_4 = 2,
    };
}
namespace via::motion::SubExJointRemapOutput {
    enum class Axis : int64_t {
        X = 0,
        Y = 1,
        Z = 2,
    };
}
namespace app::ropeway::ObjectStopControl {
    enum class ControlLayer : int64_t {
        DEFAULT = 0,
        SELF = 1,
        DELETE_ = 2,
        EDITED_BY_SCENE = 3,
        SURVIVOR_REST_COLLECTOR = 4,
        SURVIVOR_COND_VALID = 5,
        SCE_CTRL = 6,
        GIMMICK_MANAGER_SUSPEND = 7,
        TIMELINE_EVENT = 8,
        TIMELINE_EVENT_STOP = 9,
        EM_CTRL = 10,
        EM_RESTRICT_MANAGER = 11,
        FSM_OSC = 12,
        FSM_DEACTIVE = 13,
        OBJ_REST_CTRL = 14,
        OBJ_REST_GMK_INUSE = 15,
        GIMMICK_UTIL = 16,
        ENVIRONMENT_STANDBY_CONTROL = 17,
        EM_CTRL_START_EVENT = 18,
        ENV_STANDBY_MANAGER = 19,
        PROBE_CONTROL = 20,
    };
}
namespace app::ropeway::survivor::npc::NpcOrderDefine {
    enum class UniqueCommandKind : int64_t {
        Pl3000_EnableCrouch = 0,
        Pl3000_ForceCrouch = 1,
        Invalid = 2,
    };
}
namespace app::ropeway::GroundFixer {
    enum class AdjustMode : int64_t {
        RAY_FIT = 0,
        GRAVITY = 1,
        ADJUST_GRAVITY = 2,
        SIMPLE_GRAVITY = 3,
    };
}
namespace via::hid::VrTrackerResultData {
    enum class RearStatus : int64_t {
        NotReady = 0,
        Ready = 1,
        NotSupported = 2,
    };
}
namespace via::hid::hmd::MorpheusDevice {
    enum class ReprojectionType : int64_t {
        Default = 0,
        WithOverlay = 1,
    };
}
namespace app::ropeway::fsmv2 {
    enum class ValueSwitchType : int64_t {
        KEEP = 0,
        RESET = 1,
        ON = 2,
        OFF = 3,
    };
}
namespace via::behaviortree {
    enum class RestartType : int64_t {
        ExecuteOn = 0,
        ExecuteOff = 1,
        UseResource = 2,
        Ignore = 3,
    };
}
namespace app::ropeway::enemy::em7200::Em7200Think {
    enum class ThrowTaregtTypeEnum : int64_t {
        CAPSULE_ONLY = 0,
        LARGE_ONLY = 1,
        ALL = 2,
    };
}
namespace app::ropeway::enemy::common::behaviortree::condition::CaseRange {
    enum class RANGE_TYPE : int64_t {
        RANGE_1 = 0,
        RANGE_2 = 1,
        RANGE_3 = 2,
        RANGE_4 = 3,
    };
}
namespace app::ropeway::enemy::em0000::MotionPattern {
    enum class Standup : int64_t {
        StandupF = 0,
        StandupB = 1,
        StandupL = 2,
        StandupR = 3,
    };
}
namespace app::ropeway::gamemastering::UIMapManager {
    enum class MapPartsChangeState : int64_t {
        INVALID = 0xFFFFFFFF,
        RPD_HALL_WEST_SHUTTER = 0,
        RPD_HALL_EAST_SHUTTER = 1,
        RPD_HALL_HIDING_STAIRS = 2,
        RPD_ROOM_UNDER_STAIRS = 3,
        RPD_1F_DOOR_BROKEN = 4,
        RPD_2F_SHUTTER = 5,
        RPD_LIBRARY_MOVED_BOOKSHELF = 6,
        RPD_LIBRARY_BROKEN_FLOOR = 7,
        RPD_GUARD_ROOM_SHUTTER = 8,
        RPD_B1_SHUTTER = 9,
        RPD_PARKING_SHUTTER = 10,
        RPD_PARKING_WALL_BROKEN = 11,
        RPD_PRESS_ROOM_WALL_BROKEN = 12,
        RPD_OUTSIDE_STAIRS_BROKEN = 13,
        RPD_3F_STAIRS = 14,
        RPD_JAIL_PUZZLE_SOLVED = 15,
        RPD_JAIL_ZOMBIE_PANIC = 16,
        UG_G1_BATTLE_START = 17,
        UG_G1_BATTLE_AFTER = 18,
        UG_BRIDGE_MOVED = 19,
        ORPHAN_ASYLUM_LADDER = 20,
        WASTE_B1_BRIDGE_MOVED = 21,
        WASTE_B2_SHUTTER = 22,
        WASTE_B3_SHUTTER_A = 23,
        WASTE_B3_SHUTTER_B = 24,
        WASTE_B3_SHUTTER_C = 25,
        LABO_LADDER = 26,
        LABO_G3_BATTLE_START = 27,
        LABO_UG_UNDER_LADDER = 28,
        LABO_UG_ANOTHER_ROOT = 29,
        LABO_CENTER_EV_UNLOCK = 30,
        LABO_NORTH_BRIDGE_UNLOCK = 31,
        LABO_EAST_BRIDGE_UNLOCK = 32,
        LABO_WEST_BRIDGE_UNLOCK = 33,
        WASTE_B2_BRIDGE_MOVED = 34,
        RPD_JAIL_OPEN_WEST_DOOR = 35,
        PIPE_LADDER = 36,
        LABO_OPEN_PARTITION = 37,
        RPD_2F_ELEVATOR_CLAIRE = 38,
        FOURTH_TOFU_MODE = 39,
        WASTE_B1_OPEN_INCINERATION = 40,
        WASTE_B1_HACKING_ROOM_AFTER = 41,
        WASTE_B2_NO_CABLECAR = 42,
        WASTE_G2_BREAK_WALL = 43,
        RPD_FIRST_OPEN_MAP = 44,
        WASTE_B1_ELEVATOR = 45,
        WASTE_G2_BATTLE_AFTER = 46,
        LABO_WEST_BRIDGE_BROKEN = 47,
        RPD_2F_STARS_DOOR_OFF = 48,
        GUNSHOP_DOOR_A_OFF = 49,
        GUNSHOP_DOOR_B_OFF = 50,
        GUNSHOP_DOOR_C_OFF = 51,
        ORPHAN_ASYLUM_DOOR_OFF = 52,
        LABO_UG_TYRANT_BATTLE = 53,
        LABO_UG_G4_BATTLE = 54,
        LABO_UG_G5_BATTLE = 55,
        WASTE_B2_CHESS_DOOR_OFF = 56,
        WASTE_B2_GARBAGE_DOOR_OFF = 57,
        WASTE_B1_LADDER_OFF = 58,
        LABO_UG_LIFT_MOVED = 59,
        LABO_UG_TURN_TABLE_MOVED = 60,
        LABO_NORTH_DUCT_ENTERED = 61,
        WASTE_B2_POWERROOM_DOOR_BROKEN = 62,
        LABO_RECLOSED_PARTITION = 63,
        MAX = 64,
    };
}
namespace app::ropeway {
    enum class IkSpineKind : int64_t {
        SPINE = 0,
        ATTITUDE = 1,
    };
}
namespace via::motion::CppSampleChild {
    enum class Test : int64_t {
        A = 0,
        B = 1,
        C = 2,
    };
}
namespace via::render::LightVolumeDecal {
    enum class NormalRoughnessMapType : int64_t {
        Normal = 0,
        NormalRoughness = 1,
    };
}
namespace app::ropeway::survivor::SurvivorCharacterController {
    enum class ShapeCategory : int64_t {
        Default = 0,
        Idle = 1,
        Hold = 2,
        Damage = 3,
        Holded = 4,
        Gimmick = 5,
    };
}
namespace via::effect::detail {
    enum class FrontDirection : int64_t {
        X = 0,
        Y = 1,
        Z = 2,
    };
}
namespace app::ropeway::survivor::npc::NpcParam {
    enum class AngleType : int64_t {
        BACK_MOVE_START = 0,
        BACK_MOVE_END = 1,
        TURN_START = 2,
        JA_STAND_TO_WALK_START = 3,
        JA_WALK_TO_JOG_START = 4,
        JA_JOG_TO_WALK_START = 5,
        JA_WALK_TO_STAND_START = 6,
        ANY = 7,
    };
}
namespace app::ropeway::Em4000Define {
    enum class BtActionStatus : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_MOVE = 2,
        ACT_JUMP = 3,
        ACT_FALL = 4,
        ACT_FALL_END = 5,
        ACT_FALL_END_RUN = 6,
        ACT_PIVOT_TURN = 7,
        ACT_STEP = 8,
        ACT_STEP_B = 9,
        ACT_REACTION_A = 10,
        ACT_REACTION_B = 11,
        ACT_CITY_REACTION = 12,
        ACT_HOLE_WALK = 19,
        ACT_KENNEL_BARKING = 26,
        ATK_JUMP_BITE = 2000,
        DMG_KNOCKBACK_BODY_F = 3000,
        DMG_KNOCKBACK_HEAD_F = 3001,
        DMG_KNOCKBACK_BODY_BL = 3002,
        DMG_KNOCKBACK_BODY_BR = 3003,
        DMG_STUN_HEAD_F = 3004,
        DMG_BURST_F = 3006,
        DMG_BURST_B = 3007,
        DMG_BURST_L = 3008,
        DMG_BURST_R = 3009,
        DMG_DOWN_FROM_RUN = 3010,
        DMG_STAGGER_R = 3011,
        DMG_STAGGER_L = 3012,
        DMG_ATK_BURST = 3013,
        DIE_NORMAL = 5000,
        DIE_FROM_DOWN = 5001,
        DIE_FROM_RUN = 5002,
        DIE_FROM_JUMP = 5003,
        DIE_RIGID = 5006,
    };
}
namespace via::hid::PortInfo {
    enum class StateThread : int64_t {
        None = 0,
        Uninitialized = 1,
        Initializing = 2,
        Idle = 3,
        Executing = 4,
        Terminated = 5,
        Max = 6,
    };
}
namespace via::effect::detail {
    enum class NodeBillboardShapeType : int64_t {
        Box = 0,
        Sphere = 1,
        Num = 2,
    };
}
namespace via::storage::saveService {
    enum class SaveDataEncryptionType : int64_t {
        None = 0,
        AutoStrong = 1,
        XOR = 2,
        BlowFish = 3,
    };
}
namespace app::ropeway::timeline::TimelineEventSettingTable {
    enum class ActorType : int64_t {
        Survivor = 0,
        Enemy = 1,
        Camera = 2,
        Fade = 3,
        SetModel = 4,
        GUI = 5,
        Invalid = 6,
    };
}
namespace app::ropeway::emXXXXParam {
    enum class EM0000_MOTION_LIST : int64_t {
        Eat_Face_Down_00 = 4500,
        Eat_Face_Down_01 = 4502,
        Eat_Face_Up_00 = 4510,
    };
}
namespace app::ropeway::AssetPackageManager::PackageInfo {
    enum class StateType : int64_t {
        SLEEP = 0,
        LOADING = 1,
        READY = 2,
    };
}
namespace System::Runtime::CompilerServices {
    enum class CompilationRelaxations : int64_t {
        NoStringInterning = 0,
    };
}
namespace app::ropeway::ReNetData::PuzzlePlayData {
    enum class PuzzleType : int64_t {
        PUZZLE_00 = 0,
        PUZZLE_01 = 1,
        PUZZLE_02 = 2,
        PUZZLE_03 = 3,
        PUZZLE_04 = 4,
        PUZZLE_05 = 5,
        PUZZLE_06 = 6,
        PUZZLE_07 = 7,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0100_MONTAGE : int64_t {
        ID000 = 0,
        ID001 = 1,
        ID002 = 2,
        ID003 = 3,
        ID004 = 4,
        ID005 = 5,
        ID006 = 6,
        ID007 = 7,
        ID008 = 8,
        ID009 = 9,
        ID010 = 10,
        ID011 = 11,
        ID012 = 12,
        ID013 = 13,
        ID014 = 14,
        ID015 = 15,
        ID016 = 16,
        ID200 = 17,
        ID201 = 18,
        ID202 = 19,
        ID203 = 20,
        ID204 = 21,
        ID300 = 22,
        ID301 = 23,
        ID302 = 24,
        ID303 = 25,
        ID304 = 26,
        ID800 = 27,
        ID801 = 28,
        ID802 = 29,
        ID803 = 30,
        ID804 = 31,
        ID999 = 32,
    };
}
namespace app::ropeway::enemy::em3000::ActionStatus {
    enum class Type : int64_t {
        GROUND = 0,
        NOT_GROUND = 1,
    };
}
namespace app::ropeway::camera::CameraShakeData {
    enum class ShakeType : int64_t {
        NONE = 0,
        TIMER = 1,
        SINE = 2,
    };
}
namespace via::motion {
    enum class SmoothDissimilarityFrameType : int64_t {
        Default = 0,
        EndFrame = 1,
    };
}
namespace System {
    enum class AttributeTargets : int64_t {
        Assembly = 1,
        Module = 2,
        Class = 4,
        Struct = 8,
        Enum = 16,
        Constructor = 32,
        Method = 64,
        Property = 128,
        Field = 256,
        Event = 512,
        Interface = 1024,
        Parameter = 2048,
        Delegate = 4096,
        ReturnValue = 8192,
        All = 16383,
    };
}
namespace app::ropeway::gimmick::action::AreaAccessSensor {
    enum class NPCType : int64_t {
        ADA = 0,
        SHERRY = 1,
    };
}
namespace app::ropeway::survivor::fsmv2::condition::SurvivorCheckWeaponFsmCondition::CheckWeaponParam {
    enum class CheckKind : int64_t {
        None = 0,
        Type = 1,
        Category = 2,
        EquipCategory = 4,
    };
}
namespace app::ropeway::enemy::em6100::fsm::tag {
    enum class StateAttr : int64_t {
        Swim = 531802718,
        Attack = 445732949,
        Jump = 4231658239,
        Dead = 527691310,
        Turn = 1801898850,
        Float = 849320574,
    };
}
namespace app::ropeway::gamemastering::TimelineEventManager::EventOrderRequeest {
    enum class AsseptingStatus : int64_t {
        WaitSceneLoad = 0,
        Accepted = 1,
        Untreated = 2,
    };
}
namespace app::ropeway::gui::RogueInventorySlotBehavior {
    enum class FatSlotWeaponCandidate : int64_t {
        WP0000 = 1,
        WP0200 = 3,
        WP0800 = 9,
        WP1000 = 11,
        WP2000 = 21,
        WP3000 = 31,
    };
}
namespace app::ropeway::OptionManager {
    enum class AmbientOcclusion : int64_t {
        OFF = 0,
        SSAO_FLEXIBLE = 1,
        SSAO = 2,
        HDAO = 3,
        HBAO = 4,
    };
}
namespace app::ropeway::gui::FloorMapSt4DBehavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        Door_3_1_003 = 1,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class HandsState : int64_t {
        BOTH_HANDS = 0,
        L_HANDS_ONLY = 1,
        R_HANDS_ONLY = 2,
        NO_HANDS = 3,
    };
}
namespace via::fsm {
    enum class TransitionAttribute : int64_t {
        Warp = 0,
        IgnorePuppetMode = 1,
        TransitionAttributeBitNum = 32,
    };
}
namespace via::render::RenderTargetOperator {
    enum class Op : int64_t {
        None = 0,
        Add = 1,
        Multiply = 2,
        Maximum = 16,
        Minimum = 17,
    };
}
namespace app::ropeway::survivor::fsmv2::action::SurvivorUpdateWallMaterialAction {
    enum class Timing : int64_t {
        Invalid = 0xFFFFFFFF,
        None = 0,
        ActionStart = 1,
        ActionEnd = 2,
    };
}
namespace app::ropeway::gamemastering::RogueWarpManager {
    enum class RNO : int64_t {
        IDLE = 0,
        REQUEST_INITIALIZE = 1,
        FADEOUT_WAIT = 2,
        UNLOAD_WAIT = 3,
        WAIT_GUI_LOADING = 4,
        OPEN_GUI_LOADING = 5,
        UNLOAD_SCENE_FOLDER = 6,
        LOAD_SCENE_FOLDER = 7,
        LOAD_SCENE_FOLDER_WAIT = 8,
        FINISH = 9,
        FINISH_WAIT = 10,
        EXEC_STAGE_RESET = 11,
    };
}
namespace via::gui::detail {
    enum class ResolutionAdjust : int64_t {
        StretchAlways = 0,
        StretchExpanding = 1,
        StretchShrinking = 2,
        FitSmallRatioAxisAlways = 3,
        FitSmallRatioAxisExpanding = 4,
        FitSmallRatioAxisShrinking = 5,
        FitLargeRatioAxisAlways = 6,
        FitLargeRatioAxisExpanding = 7,
        FitLargeRatioAxisShrinking = 8,
        None = 9,
        Max = 10,
    };
}
namespace via::render {
    enum class FillMode : int64_t {
        Wireframe = 1,
        Solid = 2,
        Num = 3,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::EmCommonFsmAction_SetMotionVariationPatten {
    enum class SetTypePattern : int64_t {
        Immediate = 0,
        Random = 1,
        PatternTable = 2,
    };
}
namespace app::ropeway::gamemastering::item {
    enum class Type : int64_t {
        Invalid = 0,
        Heal = 1,
        Attack = 2,
        Key = 3,
        Damage = 4,
    };
}
namespace app::ropeway::gimmick::option::KariCastingBoardSettings {
    enum class Phase : int64_t {
        None = 0,
        SurvivorManager = 1,
        NpcManager = 2,
        Completed = 3,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class EnemyManagePhase : int64_t {
        FASTEST = -61,
        Manager = -60,
        AttackInitiative = -59,
        LATEST = -58,
    };
}
namespace via::nnfc::nfp {
    enum class NfpSequence : int64_t {
        None = 0,
        Detection = 1,
        GetTagInfo = 2,
        Mount = 3,
        GetNfpData = 4,
        OpenApplicationArea = 5,
        CreateApplicationData = 6,
        GetApplicationData = 7,
        WriteApplicationData = 8,
    };
}
namespace app::ropeway::gamemastering::AchievementDefine {
    enum class ID : int64_t {
        PLATINUM_000 = 0,
        GOLD_000 = 1,
        GOLD_001 = 2,
        GOLD_002 = 3,
        GOLD_003 = 4,
        SILVER_000 = 5,
        SILVER_001 = 6,
        SILVER_002 = 7,
        SILVER_003 = 8,
        SILVER_004 = 9,
        SILVER_005 = 10,
        SILVER_006 = 11,
        SILVER_007 = 12,
        SILVER_008 = 13,
        BRONZE_000 = 14,
        BRONZE_001 = 15,
        BRONZE_002 = 16,
        BRONZE_003 = 17,
        BRONZE_004 = 18,
        BRONZE_005 = 19,
        BRONZE_006 = 20,
        BRONZE_007 = 21,
        BRONZE_008 = 22,
        BRONZE_009 = 23,
        BRONZE_010 = 24,
        BRONZE_011 = 25,
        BRONZE_012 = 26,
        BRONZE_013 = 27,
        BRONZE_014 = 28,
        BRONZE_015 = 29,
        BRONZE_016 = 30,
        BRONZE_017 = 31,
        BRONZE_018 = 32,
        BRONZE_019 = 33,
        BRONZE_020 = 34,
        BRONZE_021 = 35,
        BRONZE_022 = 36,
        BRONZE_023 = 37,
        BRONZE_024 = 38,
        BRONZE_025 = 39,
        BRONZE_026 = 40,
        BRONZE_027 = 41,
    };
}
namespace app::ropeway::gui::DialogBehavior {
    enum class Result : int64_t {
        NO = 0,
        YES = 1,
    };
}
namespace app::ropeway::enemy::em6200 {
    enum class PressGroup : int64_t {
        Default = 0,
        Sleep = 1,
    };
}
namespace app::ropeway::enemy::em0000::MotionPattern {
    enum class Hold : int64_t {
        Front = 0,
        Left = 1,
        Right = 2,
        Back_LeftTurn = 3,
        Back_RightTurn = 4,
    };
}
namespace app::ropeway::gui::FloorMapSt1BBehavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        Door_4_1_011 = 1,
        Door_4_1_012 = 2,
        Door_4_1_013 = 3,
        sm40_135_OrphanGate01A = 4,
        Door_ByGunShop = 5,
    };
}
namespace app::ropeway::effect::script::RenderTargetTextureManager {
    enum class RTTRequestState : int64_t {
        Invalid = 0,
        Live = 1,
        Dead = 2,
        Light = 3,
    };
}
namespace app::ropeway::enemy::em0000::fsmv2::action::Em0000MFsmAction_StartTimer {
    enum class TimerTypeDefine : int64_t {
        KNOCK_DOOR = 0,
        FLASH = 1,
    };
}
namespace via::storage {
    enum class CompressionAlgorithm : int64_t {
        Deflate = 0,
    };
}
namespace app::ropeway::enemy::em6200::MotionPattern {
    enum class DirLR : int64_t {
        Left = 0,
        Right = 1,
    };
}
namespace via::render::command::NVNExtention {
    enum class ExtentionType : int64_t {
        ResolveDepthBuffer = 0,
        SetTiledCacheTileSize = 1,
        TiledCacheActionEnable = 2,
        TiledCacheActionDisable = 3,
        TiledCacheActionFlush = 4,
        TiledCacheActionFlushNoTiling = 5,
        TiledCacheActionEnableRenderTargetBinning = 6,
        TiledCacheActionDisableRenderTargetBinning = 7,
        SaveZCullData = 8,
        RestoreZCullData = 9,
    };
}
namespace app::ropeway::fsmv2::EnemyCheck {
    enum class CheckType : int64_t {
        FINISH_JACKED = 0,
    };
}
namespace via::Frustum {
    enum class HIT_TYPE : int64_t {
        HIT_OUTSIDE = 0,
        HIT_INSIDE = 1,
        HIT_INTERSECT = 2,
    };
}
namespace via::motion::IkTrain {
    enum class TrainRotate : int64_t {
        Default = 0,
        DiffRot = 1,
    };
}
namespace via::effect::detail {
    enum class FrontDirectionType : int64_t {
        ParallelCamera = 0,
        ToCamera = 1,
    };
}
namespace app::ropeway::gui::RogueInventoryDetailBehavior {
    enum class ItemModelState : int64_t {
        Ready = 0,
        Wait = 1,
        Success = 2,
        Failure = 3,
    };
}
namespace via::render::command::XB1Extention {
    enum class ExtentionType : int64_t {
        ExtentionType_PushToEsram = 0,
        ExtentionType_PopFromEsram = 1,
        ExtentionType_DmaTransfer_DtoE = 2,
        ExtentionType_DmaTransfer_EtoD = 3,
        ExtentionType_ResolveScratchShadow = 4,
        ExtentionType_CopyShadowCache = 5,
        ExtentionType_ResumarizeHTile = 6,
        ExtentionType_ExecuteDMA = 7,
        ExtentionType_4KCheckerBoardRenderingEnable = 8,
        ExtentionType_4KCheckerBoardRenderingEvenFrameEnable = 8,
        ExtentionType_4KCheckerBoardRenderingOddFrameEnable = 9,
        ExtentionType_TemporalAntialiasing4KCheckerBoardRenderingEvenFrameEnable = 10,
        ExtentionType_TemporalAntialiasing4KCheckerBoardRenderingOddFrameEnable = 11,
        ExtentionType_4KCheckerBoardRenderingDisable = 12,
        ExtentionType_AlphaUnroll_Enable = 13,
        ExtentionType_AlphaUnroll_Disable = 14,
    };
}
namespace via::dynamics {
    enum class RigidBodyState : int64_t {
        Disable = 0,
        Static = 1,
        KeyFramed = 2,
        Dynamic = 3,
        Phantom = 4,
        Invalid = 5,
        Max = 6,
    };
}
namespace app::ropeway::gui::FloorMapSt11Behavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        Door_5_1_015 = 1,
        sm40_130_OrphanageDoor01A = 2,
        Door_5_1_016 = 3,
        Door_5_1_020 = 4,
        sm40_135_OrphanGate01A = 5,
    };
}
namespace app::ropeway::gui::KeyBindBehavior {
    enum class SlotType : int64_t {
        Primary = 0,
        Secondary = 1,
    };
}
namespace via::gui::MessageTag {
    enum class Attr : int64_t {
        None = 0,
        PopEnd = 1,
        BaseColor = 2,
        Icon = 4,
        Ruby = 8,
    };
}
namespace via::areamap::test::DistanceToSelf {
    enum class Condition : int64_t {
        Mode_Closest = 0,
        Mode_Farthest = 1,
    };
}
namespace app::ropeway::FadeController {
    enum class ColorType : int64_t {
        BLACK = 0,
        WHITE = 1,
        NUM = 2,
    };
}
namespace app::MathEx {
    enum class SliceType : int64_t {
        Round = 0,
        Pizza = 1,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class EventSetAction : int64_t {
        CUSTOM_JACKED = 0,
        CUSTOM_JACKED_DEAD = 1,
        TIMELINE = 2,
        OPENING_EM0000_51 = 3,
        OPENING_EM0000_52 = 4,
        OPENING_EM0000_53 = 5,
        OPENING_EM0000_54 = 6,
        OPENING_EM0000_55 = 7,
        OPENING_EM0000_56 = 8,
        OPENING_EM0000_57 = 9,
    };
}
namespace via::motion::IkSpineConformGround {
    enum class ROT_TYPE : int64_t {
        SPEED = 0,
        DAMPING = 1,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class DisorderKind : int64_t {
        DOWNED = 0,
        LAYING = 1,
        FAKEDEAD = 2,
        FORBID_FAKEDEAD = 3,
        FORBID_STANDUP_BY_ROLE = 4,
        FORBID_STANDUP_BY_STEP = 5,
        FORBID_STANDUP_BY_TIMER = 6,
        BURNUP_BY_ROLE_STANDBY = 7,
        BURNUP_BY_ROLE = 8,
        BURNUP_BY_ROLE_FINISHED = 9,
        HELLFIRE_STANDBY = 10,
        EATEN_BY_ROLE = 11,
        FIND_VOICE = 12,
        SILENT_LOOKAT = 13,
        DISABLE_ORDINARY_PRESS = 14,
        DISABLE_AIMASSIST = 15,
        FORBID_STUMBLE = 16,
        FORBID_BREAK_LEG = 17,
        FORBID_BREAK_LEG_REACTION = 18,
        DEAD_WITH_CAPTURED_POSE = 19,
        INTO_INTERACTIVE_GIMMICK = 20,
        INTO_INTERACTIVE_STEP = 21,
        LAST_KNIFE_CONST2NECK = 22,
    };
}
namespace app::ropeway::gui::FloorMapSt4DBehavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_4_750_0c = 1,
        Room_4_750_0d = 2,
        Room_4_751_0f = 3,
    };
}
namespace app::ropeway::gamemastering::AchievementDefine {
    enum class HeroStatID : int64_t {
        ClearGame = 0,
        KillZombie = 1,
        BittenByZombie = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class FootState : int64_t {
        BOTH_FOOT = 0,
        L_FOOT_ONLY = 1,
        R_FOOT_ONLY = 2,
        NO_FOOT = 3,
    };
}
namespace via::motion {
    enum class BlendType : int64_t {
        Normal = 0,
        Layer = 1,
        Interpolation = 2,
        Switch = 3,
    };
}
namespace via::render {
    enum class InputElementFormat : int64_t {
        Float1 = 0,
        Float2 = 1,
        Float3 = 2,
        Float4 = 3,
        Half2 = 4,
        Half4 = 5,
        UByte4 = 6,
        Byte4 = 7,
        NormUByte4 = 8,
        NormByte4 = 9,
        UShort4 = 10,
        Short4 = 11,
        UShort2 = 12,
        Short2 = 13,
        NormUDec3 = 14,
        UDec3 = 15,
        Int4 = 16,
    };
}
namespace app::ropeway::Mimic {
    enum class TargetSelectMode : int64_t {
        ByBehavior = 0,
        Parent = 1,
        TargetRef = 2,
        LocalPath = 3,
        FullPath = 4,
    };
}
namespace via::motion {
    enum class WrapMode : int64_t {
        Default = 0,
        Once = 1,
        Loop = 2,
        TurnBack = 3,
        LoopTurnBack = 4,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::EmCommonMFsmAction_GameRankMotionSpeedControl {
    enum class ExecMode : int64_t {
        ACTION = 0,
        SEQUENCE = 1,
    };
}
namespace app::ropeway::fsmv2::EnemyConditionCheck {
    enum class ConditionResult : int64_t {
        NONE = 0,
        ACTION_END = 1,
        ACTION_FAILED = 2,
    };
}
namespace via::render {
    enum class TextureFormat : int64_t {
        Unknown = 0,
        R32G32B32A32Typeless = 1,
        R32G32B32A32Float = 2,
        R32G32B32A32Uint = 3,
        R32G32B32A32Sint = 4,
        R32G32B32Typeless = 5,
        R32G32B32Float = 6,
        R32G32B32Uint = 7,
        R32G32B32Sint = 8,
        R16G16B16A16Typeless = 9,
        R16G16B16A16Float = 10,
        R16G16B16A16Unorm = 11,
        R16G16B16A16Uint = 12,
        R16G16B16A16Snorm = 13,
        R16G16B16A16Sint = 14,
        R32G32Typeless = 15,
        R32G32Float = 16,
        R32G32Uint = 17,
        R32G32Sint = 18,
        R32G8X24Typeless = 19,
        D32FloatS8X24Uint = 20,
        R32FloatX8X24Typeless = 21,
        X32TypelessG8X24Uint = 22,
        R10G10B10A2Typeless = 23,
        R10G10B10A2Unorm = 24,
        R10G10B10A2Uint = 25,
        R11G11B10Float = 26,
        R8G8B8A8Typeless = 27,
        R8G8B8A8Unorm = 28,
        R8G8B8A8UnormSrgb = 29,
        R8G8B8A8Uint = 30,
        R8G8B8A8Snorm = 31,
        R8G8B8A8Sint = 32,
        R16G16Typeless = 33,
        R16G16Float = 34,
        R16G16Unorm = 35,
        R16G16Uint = 36,
        R16G16Snorm = 37,
        R16G16Sint = 38,
        R32Typeless = 39,
        D32Float = 40,
        R32Float = 41,
        R32Uint = 42,
        R32Sint = 43,
        R24G8Typeless = 44,
        D24UnormS8Uint = 45,
        R24UnormX8Typeless = 46,
        X24TypelessG8Uint = 47,
        R8G8Typeless = 48,
        R8G8Unorm = 49,
        R8G8Uint = 50,
        R8G8Snorm = 51,
        R8G8Sint = 52,
        R16Typeless = 53,
        R16Float = 54,
        D16Unorm = 55,
        R16Unorm = 56,
        R16Uint = 57,
        R16Snorm = 58,
        R16Sint = 59,
        R8Typeless = 60,
        R8Unorm = 61,
        R8Uint = 62,
        R8Snorm = 63,
        R8Sint = 64,
        A8Unorm = 65,
        R1Unorm = 66,
        R9G9B9E5Sharedexp = 67,
        R8G8B8G8Unorm = 68,
        G8R8G8B8Unorm = 69,
        Bc1Typeless = 70,
        Bc1Unorm = 71,
        Bc1UnormSrgb = 72,
        Bc2Typeless = 73,
        Bc2Unorm = 74,
        Bc2UnormSrgb = 75,
        Bc3Typeless = 76,
        Bc3Unorm = 77,
        Bc3UnormSrgb = 78,
        Bc4Typeless = 79,
        Bc4Unorm = 80,
        Bc4Snorm = 81,
        Bc5Typeless = 82,
        Bc5Unorm = 83,
        Bc5Snorm = 84,
        B5G6R5Unorm = 85,
        B5G5R5A1Unorm = 86,
        B8G8R8A8Unorm = 87,
        B8G8R8X8Unorm = 88,
        R10G10B10xrBiasA2Unorm = 89,
        B8G8R8A8Typeless = 90,
        B8G8R8A8UnormSrgb = 91,
        B8G8R8X8Typeless = 92,
        B8G8R8X8UnormSrgb = 93,
        Bc6hTypeless = 94,
        Bc6hUF16 = 95,
        Bc6hSF16 = 96,
        Bc7Typeless = 97,
        Bc7Unorm = 98,
        Bc7UnormSrgb = 99,
        ForceUint = 0xFFFFFFFF,
    };
}
namespace app::ropeway::gamemastering::MainFlowManager {
    enum class ScenarioType : int64_t {
        LEON_A = 0,
        CLAIRE_A = 1,
        LEON_B = 2,
        CLAIRE_B = 3,
        HUNK = 4,
        TOFU = 5,
        ROGUE = 6,
        L_A = 7,
        L_B = 8,
        L_C = 9,
        L_D = 10,
        MAX = 11,
        INVALID = 12,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class ActionTargetType : int64_t {
        DIRECT = 0,
        NAVIGATION = 1,
    };
}
namespace via::areamap::AreaMemory {
    enum class Mode : int64_t {
        Seconds = 0,
        Frames = 1,
    };
}
namespace via::motion::JointRemapValue::RemapValueItem {
    enum class InputType : int64_t {
        Trans = 0,
        Rot = 1,
        Scale = 2,
        Cone = 3,
    };
}
namespace via::hid::mouse::impl {
    enum class ManipulatorClientType : int64_t {
        NULL_ = 0,
        DirectInput = 1,
        RawInput = 2,
        WindowMessage = 3,
        GlobalParameter = 4,
        RuntimeDefault = 5,
        ToolDefault = 6,
    };
}
namespace app::ropeway::camera {
    enum class CameraInterpolateCurve : int64_t {
        LINEAR = 0,
        EASE_IN_ = 1,
        EASE_OUT = 2,
        EASE_INOUT = 3,
    };
}
namespace app::ropeway::AimUserDataBase {
    enum class GroupKind : int64_t {
        Enemy = 0,
        Survivor = 1,
        Gimmick = 2,
    };
}
namespace app::ropeway::KnifePartsController {
    enum class vitalKinfe : int64_t {
        Cautiuon = 0,
        Denger = 1,
    };
}
namespace app::ropeway::gimmick::option::MovePositionSettings {
    enum class DispSetType : int64_t {
        NO_TOUCH = 0,
        DISP_AT_START = 1,
        HIDE_AT_END = 2,
    };
}
namespace via::navigation::NodeFilterInfo {
    enum class FilterType : int64_t {
        UseOn = 0,
        UseOff = 1,
    };
}
namespace app::ropeway::gui::ItemPortableSafeBehavior {
    enum class MaterialEnum : int64_t {
        Base = 0,
        Lamp1 = 1,
        Lamp2 = 2,
        Lamp3 = 3,
        Lamp4 = 4,
        Lamp5 = 5,
        Lamp6 = 6,
        Lamp7 = 7,
        Lamp8 = 8,
        Lamp11 = 9,
        Lamp12 = 10,
        Lamp13 = 11,
        Lamp14 = 12,
        Lamp15 = 13,
        Lamp16 = 14,
        Lamp17 = 15,
        Lamp18 = 16,
    };
}
namespace via::clr {
    enum class EnumU2 : int64_t {
        Dummy = 0,
    };
}
namespace app::ropeway::PrefabExtension {
    enum class InstantiateExecuteResult : int64_t {
        Success = 0,
        FailureByPrefabUnloaded = 1,
        FailureByInstantiate = 2,
        FailureByInvalidSummoner = 3,
    };
}
namespace app::ropeway::gimmick::option::GimmickCameraSettings::FixCameraSetting::GuiParameter {
    enum class ActionType : int64_t {
        OPEN = 0,
        CLOSE = 1,
    };
}
namespace app::ropeway::EnemyDataManager::DynamicMotionBankInfo {
    enum class LoadStatus : int64_t {
        STANDBY = 0,
        ACTIVATE = 1,
        WAITING = 2,
        LOADING = 3,
        CREATE_OBJECT = 4,
        READY = 5,
        UNLOADING = 6,
    };
}
namespace app::ropeway::gimmick::action::GimmickShutterG2Break {
    enum class MotionCoreId : int64_t {
        Base = 0,
        MotionCoreIdNum = 1,
    };
}
namespace app::ropeway::InputSystem::CommandPart {
    enum class Passage : int64_t {
        DOWN = 0,
        ON = 1,
        UP = 2,
        OFF = 3,
        KEEP = 4,
    };
}
namespace via::motion::script::FootEffectController {
    enum class WwiseSpecialMaterialID : int64_t {
        NONE = 0,
        WATER = 1,
    };
}
namespace app::ropeway::gimmick::GimmickCollisionType {
    enum class Type : int64_t {
        TYPE_A = 0,
        TYPE_B = 1,
        TYPE_C = 2,
        TYPE_D = 3,
        TYPE_E = 4,
        TYPE_F = 5,
        TYPE_G = 6,
        TERRAIN_ = 7,
        SYSTEM = 8,
    };
}
namespace app::ropeway::gui::RogueInventorySlotBehavior {
    enum class ExCombinationType : int64_t {
        Invalid = 0xFFFFFFFF,
        StorageToStorage = 0,
        StorageToSlot = 1,
        SlotToStorage = 2,
        SlotToSlot = 3,
    };
}
namespace app::ropeway::gui::SelectHDRBrightnessBehavior {
    enum class EndType : int64_t {
        SELECTED = 0,
        CANCELED = 1,
    };
}
namespace via::clr {
    enum class TokenKind : int64_t {
        Type = 0,
        Method = 1,
        Field = 2,
    };
}
namespace via::dynamics {
    enum class ConstraintType : int64_t {
        Invalid = 0,
        BallJoint = 1,
        ConeTwist = 2,
        Hinge = 3,
        Ragdoll = 4,
        Max = 5,
    };
}
namespace app::ropeway::InputSystem {
    enum class MouseCursor : int64_t {
        Hide = 0,
        Show = 1,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0000_MONTAGE_PARTS_FACE : int64_t {
        FACE00 = 0,
        FACE01 = 1,
        FACE02 = 2,
        FACE03 = 3,
        FACE04 = 4,
        FACE05 = 5,
        FACE06 = 6,
        FACE07 = 7,
        FACE08 = 8,
        FACE09 = 9,
        FACE10 = 10,
        FACE11 = 11,
        FACE12 = 12,
        FACE13 = 13,
        FACE14 = 14,
        FACE70 = 15,
        FACE71 = 16,
        FACE72 = 17,
        FACE73 = 18,
        FACE74 = 19,
        FACE75 = 20,
        FACE76 = 21,
        NONE = 22,
    };
}
namespace app::ropeway::timeline {
    enum class UpdateState : int64_t {
        Setup = 0,
        Loading = 1,
        PreStart = 2,
        Standby = 3,
        PlayStart = 4,
        Play = 5,
        Cleanup = 6,
        Finish = 7,
    };
}
namespace via::Entry {
    enum class Type : int64_t {
        Initialize = 0,
        Setup = 1,
        Start = 2,
        Update = 3,
        Terminate = 4,
        Finalize = 5,
        Max = 6,
    };
}
namespace via::browser {
    enum class OpenError : int64_t {
        None = 0,
        InvalidArgument = 1,
        Busy = 2,
        NativeApi = 3,
        NotProvided = 4,
    };
}
namespace via::storage {
    enum class CompressionServiceTicketStatus : int64_t {
        Invalid = 0,
        Requested = 1,
        Running = 2,
        Finished = 3,
    };
}
namespace app::ropeway::AttackUserDataToEnemy {
    enum class Slip : int64_t {
        None = 0,
        Burn = 1,
        BurnUp = 2,
        Acid = 3,
        Roast = 4,
    };
}
namespace app::ropeway::enemy::em7000::Em7000Define {
    enum class WeaponType : int64_t {
        WeaponType_Pipe = 0,
        WeaponType_Spear = 1,
        WeaponType_Hammer = 2,
    };
}
namespace System::Reflection {
    enum class CallingConventions : int64_t {
        Standard = 1,
        VarArgs = 2,
        Any = 3,
        HasThis = 32,
        ExplicitThis = 64,
    };
}
namespace via::physics::CharacterController {
    enum class ContactType : int64_t {
        Ground = 0,
        Wall = 1,
        Ceiling = 2,
        Max = 3,
    };
}
namespace via::navigation::QueryObject {
    enum class WorkStatus : int64_t {
        Idle = 0,
        Work = 1,
        Finalize = 2,
    };
}
namespace app::ropeway::gui::NewInventorySlotBehavior {
    enum class CommandType : int64_t {
        Invalid = 0xFFFFFFFF,
        Use = 0,
        Look = 1,
        ShortCut = 2,
        Equip = 3,
        RemoveEquip = 4,
        EquipSub = 5,
        RemoveEquipSub = 6,
        CustomOut = 7,
        Exchange = 8,
        Combine = 9,
        Remove = 10,
        ItemBoxIN_ = 11,
        ItemBoxOut = 12,
        GetItem = 13,
        Num = 14,
    };
}
namespace app::ropeway::gui::FloorMapSt3CBehavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        Door_6_1_022 = 1,
        Door_6_1_031 = 2,
        Door_6_1_033 = 3,
        Door_6_1_034 = 4,
        Door_6_1_050 = 5,
        Door_6_1_051 = 6,
        Door_6_1_052 = 7,
        sm60_071_GarbageShutter01A = 8,
    };
}
namespace via::storage::saveService {
    enum class SaveDataOption : int64_t {
        None = 0,
        Encryption = 1,
        CheckOwner = 2,
    };
}
namespace app::ropeway::fsmv2::MotionJackedCheck {
    enum class TargetType : int64_t {
        PLAYER = 0,
    };
}
namespace app::ropeway::gui::RogueInventoryBehavior {
    enum class GetMapState : int64_t {
        Start = 0,
        Wait = 1,
    };
}
namespace app::ropeway::gui::TimelineLogoHandler {
    enum class ChangablePlayState : int64_t {
        KEEP_STATE = 0,
        FADE_IN_ = 1,
        FADE_OUT = 2,
        FORCE_DISP = 3,
        FORCE_DISABLE = 4,
    };
}
namespace via::clr {
    enum class MemberTypes : int64_t {
        Constructor = 1,
        Event = 2,
        Field = 4,
        Method = 8,
        Property = 16,
        TypeInfo = 32,
        Custom = 64,
        NestedType = 128,
    };
}
namespace app::ropeway::enemy::em6000::MotionPattern {
    enum class DirFB : int64_t {
        Front = 0,
        Back = 1,
    };
}
namespace via::render::layer {
    enum class CommonSegment : int64_t {
        UpdateConstant = 0,
        UpdateBuffer = 0,
        Cloth = 1,
        PrimitiveCS1 = 2,
        ComputeSkinning = 3,
        ComputeSkinningFence = 4,
        PrimitiveCS2 = 4,
        Wrinkle = 5,
        Stamp = 6,
        PrimitiveCS3 = 7,
        GraphicsBase = 16,
        GUI = 16,
        WrinkleGraphics = 17,
        BloodshedGraphics = 18,
        PostComputeBase = 32,
        BloodshedPostCompute = 32,
        ShallowWaterCompute = 33,
        TextureSpreadCompute = 34,
        PostGraphicsBase = 48,
        BloodshedPostGraphics = 48,
        TextureSpreadGraphics = 49,
        Lowest = 63,
    };
}
namespace via::effect::script {
    enum class PropCategory : int64_t {
        Door = 0,
        Etc = 1,
    };
}
namespace app::ropeway::environment::special::fsmv2::SpecialControlRPDFromOpening2 {
    enum class Phase : int64_t {
        Initial = 0,
        AfterFirstHide = 1,
        AfterEventControl = 2,
        ERROR_ = 3,
    };
}
namespace via::clr {
    enum class SystemMethod : int64_t {
        Equals = 0,
        GetHashCode = 1,
        Finalize = 2,
        GetType = 3,
        ToString = 4,
        CompareTo = 5,
        Compare = 6,
        DefaultExceptionHandler = 7,
        Generic_GetEnumerator = 8,
        Generic_CompareTo = 9,
        Generic_Compare = 10,
        MoveNext = 11,
        Current = 12,
        Max = 13,
    };
}
namespace via::nnfc {
    enum class DeviceRegisterResult : int64_t {
        Success = 0,
        ConflictDevice = 1,
        ConflictMode = 2,
        UnexpectedIndex = 3,
    };
}
namespace app::ropeway::fsmv2::enemy::action::Em7000FsmAction_AttackControl {
    enum class WaitReaction : int64_t {
        REACTION_A = 0,
        REACTION_B = 1,
        REACTION_C = 2,
    };
}
namespace via::render::Stamp {
    enum class TargetUV : int64_t {
        Primary = 0,
        Secondary = 1,
    };
}
namespace app::ropeway::weapon::shell::ShellDefine {
    enum class BombType : int64_t {
        Normal = 0,
        Flash = 1,
        Fire = 2,
        Acid = 3,
        Heal = 4,
        Poison = 5,
    };
}
namespace via {
    enum class ScreenShotImageOrientation : int64_t {
        None = 0,
        Rotate90 = 1,
        Rotate180 = 2,
        Rotate270 = 3,
    };
}
namespace app::ropeway::gui::GimmickKeyInputGuiBehavior {
    enum class StateType : int64_t {
        WAIT = 0,
        MANIPULATE = 1,
        SUCCESS = 2,
        FAILURE = 3,
    };
}
namespace via::motion::JointMapData {
    enum class AttributeFlags : int64_t {
        Deform = 1,
    };
}
namespace via::effect::script::EffectChainMeshAnim {
    enum class PlayTypeEnum : int64_t {
        Once = 0,
        Loop = 1,
        Pause = 2,
    };
}
namespace System::Globalization {
    enum class CultureTypes : int64_t {
        AllCultures = 0,
        FrameworkCultures = 1,
        InstalledWin32Cultures = 2,
        NeutralCultures = 3,
        ReplacementCultures = 4,
        SpecificCultures = 5,
        UserCustomCulture = 6,
        WindowsOnlyCultures = 7,
    };
}
namespace app::ropeway::PlayerDefine {
    enum class SupportItem : int64_t {
        NONE = 0,
        STUN_GUN = 1,
        GRENADE = 2,
        FLASH_GRENADE = 3,
        KNIFE = 4,
    };
}
namespace via::motion::IkMultipleDamageAction {
    enum class CalculationAddRotation : int64_t {
        SpecificJoint = 0,
        AllJoint = 1,
    };
}
namespace via::motion::IkLeg {
    enum class Lean : int64_t {
        Center = 0,
        CenterAndHeal = 1,
    };
}
namespace via::motion {
    enum class AxisDirection : int64_t {
        Undef = 0,
        X = 1,
        Y = 2,
        Z = 3,
        NX = 5,
        NY = 6,
        NZ = 7,
    };
}
namespace app::ropeway::camera {
    enum class CameraSwitchInterpolateMode : int64_t {
        CROSS_FADE = 0,
        FRONT_CROSS_FADE = 1,
    };
}
namespace via::userdata::LogicNode {
    enum class Operation : int64_t {
        And = 0,
        Or = 1,
    };
}
namespace via::storage::saveService {
    enum class SaveDataMountMode : int64_t {
        Read = 0,
        Write = 1,
        CreateWrite = 2,
    };
}
namespace via::render::ProjectionSpotLight {
    enum class LightingMode : int64_t {
        All = 0,
        Beauty = 1,
    };
}
namespace app::ropeway::gamemastering::RichpresenceDefine {
    enum class ModeType : int64_t {
        LEON = 0,
        CLAIRE = 1,
        HUNK = 2,
        TOFU = 3,
        ROGUE = 4,
        MENU = 5,
        UNKNOWN = 6,
    };
}
namespace app::ropeway::gimmick::action::GimmickHVEquipmentLever01A {
    enum class SparkState : int64_t {
        STANDBY = 0,
        SPARK = 1,
        MAX = 2,
    };
}
namespace via::effect::detail {
    enum class EmitterUpdateStatus : int64_t {
        Idle = 0,
        Running = 1,
        RunUntilDie = 2,
    };
}
namespace app::ropeway::gimmick::option::PlayerConditionCheckSettings::Param {
    enum class LogicType : int64_t {
        AND = 0,
        OR = 1,
    };
}
namespace via::network::AutoMasterBase {
    enum class Mode : int64_t {
        Independent = 0,
        ForceMaster = 1,
        ForcePuppet = 2,
        AutoMaster = 3,
        SessionHost = 4,
    };
}
namespace via::motion::script::FootEffectController {
    enum class NoHitMaterialIdType : int64_t {
        Previous = 0,
        Direct = 1,
    };
}
namespace app::ropeway::network::service::AuthorizationServiceControllerForPS4 {
    enum class Phase : int64_t {
        None = 0,
        WaitContext = 1,
        Requested = 2,
        ReqAuthCode = 3,
        WaitAuthCode = 4,
    };
}
namespace via::storage::saveService {
    enum class SaveResult : int64_t {
        NULL_ = 0,
        Doing = 1,
        Success = 2,
        Cancel = 3,
        Update_SaveFileDetail = 4,
        Failed_StartNumber = 10,
        Failed_DataNULL_ = 11,
        Failed_DataCrash = 12,
        Failed_DataSizeZero = 13,
        Failed_MetaDataCrash = 14,
        Failed_MountERROR_ = 15,
        Failed_UnMountERROR_ = 16,
        Failed_NullSaveDataERROR_ = 17,
        Failed_FileOpenERROR_ = 18,
        Failed_FileWriteERROR_ = 19,
        Failed_FileReadERROR_ = 20,
        Failed_FileRemoveERROR_ = 21,
        Failed_FileCloseERROR_ = 22,
        Failed_TempUpERROR_ = 23,
        Failed_SlotLimitOver = 24,
        Failed_SaveDataSizeMaxOver = 25,
        Failed_SaveDataSizeMinOver = 26,
        Failed_SegmentTempOpenDialog = 27,
        Failed_TransferringWriteAccess = 28,
        Failed_NoWin64 = 29,
        Failed_Steam_StartNumber = 50,
        Failed_Steam_SaveERROR_ = 51,
        Failed_Steam_LoadERROR_ = 52,
        Failed_Steam_RemoveERROR_ = 53,
        Failed_Steam_NotFireCallback = 54,
        Failed_NoSteam = 55,
        Failed_WeGame_SaveERROR_ = 56,
        Failed_WeGame_LoadERROR_ = 57,
        Failed_WeGame_RemoveERROR_ = 58,
        Failed_NoWeGame = 59,
        Failed_SceError_StartNumber = 100,
        Failed_SceInitializeERROR_ = 101,
        Failed_SceFinalizeERROR_ = 102,
        Failed_SceMountERROR_ = 103,
        Failed_SceUnMountERROR_ = 104,
        Failed_SceMountInfoERROR_ = 105,
        Failed_SceNeedFreeSpace = 106,
        Failed_SceNullSaveDataERROR_ = 107,
        Failed_SceBrokenSaveDataERROR_ = 108,
        Failed_SceDirectorySearchERROR_ = 109,
        Failed_SceNullMountModeERROR_ = 110,
        Failed_SceFileOpenERROR_ = 111,
        Failed_SceFileWriteERROR_ = 112,
        Failed_SceFileReadERROR_ = 113,
        Failed_SceFileRemoveERROR_ = 114,
        Failed_SceFileCloseERROR_ = 115,
        Failed_ScePublicDialogOpenERROR_ = 116,
        Failed_SceListDialogOpenERROR_ = 117,
        Failed_SceSystemDialogOpenERROR_ = 118,
        Failed_SceProgressDialogOpenERROR_ = 119,
        Failed_SceErrorDialogOpenERROR_ = 120,
        Failed_SceNeedFreeSpaceDialogOpenERROR_ = 121,
        Failed_SceDataCrashDialogOpenERROR_ = 122,
        Failed_SceOtherErrorDialogOpenERROR_ = 123,
        Failed_SceYesNoDialogOpenERROR_ = 124,
        Failed_SceDialogResultERROR_ = 125,
        Failed_SceDetailSetAllERROR_ = 126,
        Failed_SceDetailSetTitleERROR_ = 127,
        Failed_SceDetailSetSubTitleERROR_ = 128,
        Failed_SceDetailSetDetailERROR_ = 129,
        Failed_SceDetailSetUserParamERROR_ = 130,
        Failed_SceIconNotFound = 131,
        Failed_SceIconSetERROR_ = 132,
        Failed_SceTransferringTitleIDNULL_ = 133,
        Failed_SceTransferringFingerprintNULL_ = 134,
        Failed_SceNoPs4 = 135,
        Failed_XB1_StartNumber = 150,
        Failed_XB1_InvalidUserIndex = 151,
        Failed_XB1_SyncStorageForUserERROR_ = 152,
        Failed_XB1_NoAccess = 153,
        Failed_XB1_UpdateTooBig = 154,
        Failed_XB1_QuotaExceeded = 155,
        Failed_XB1_OutOfLocalStorage = 156,
        Failed_XB1_UpdateToStorageERROR_ = 157,
        Failed_XB1_SaveDeleteKeyERROR_ = 158,
        Failed_XB1_LoadFormStorageERROR_ = 159,
        Failed_XB1_RemoveContainerERROR_ = 160,
        Failed_XB1_GetContainerInfoERROR_ = 161,
        Failed_XB1_TransferringSCIDNULL_ = 162,
        Failed_NoXB1 = 163,
        Failed_NSW_StartNumber = 200,
        Failed_NSW_InvalidUser = 201,
        Failed_NSW_VirtualUser = 202,
        Failed_NSW_GuestUser = 203,
        Failed_NSW_NeedFreeSpace = 204,
        Failed_NoNSW = 205,
        Failed_Telemetry_StartNumber = 250,
        Failed_Telemetry_DataSize_Limit = 251,
        Failed_Telemetry_InvalidTelemetry = 252,
        Failed_Telemetry_CreateCollectionName = 253,
        Failed_Telemetry_CreateUniqueName = 254,
        Failed_Telemetry_SlotLimit = 255,
        Failed_Telemetry_NotReady = 256,
        Failed_Telemetry_TelemetryModule = 257,
        Failed_Other_StartNumber = 300,
        Failed_Simulation_ERROR_ = 301,
        Failed_SaveDataVersion_Old = 302,
        Failed_ToDo = 303,
        Failed_SaveServiceNULL_ = 304,
    };
}
namespace app::ropeway::gui::FloorMapBehavior {
    enum class DoorShape : int64_t {
        NoDisp = 0,
        Normal = 1,
        Double = 2,
    };
}
namespace app::ropeway::gui::FloorMapSt5BBehavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_5_125_0 = 1,
        Room_5_126_0 = 2,
        Room_5_127_0 = 3,
        Room_5_128_0 = 4,
        Room_5_129_0 = 5,
        Room_5_133_0b = 6,
    };
}
namespace app::ropeway::SoundColliderUserData {
    enum class BodyPartsSide : int64_t {
        Center = 0,
        Left = 1,
        Right = 2,
    };
}
namespace app::ropeway::gamemastering::EndingFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        WAIT_GUI_CREATION = 1,
        UPDATE = 2,
        FINALIZE = 3,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace app::ropeway::enemy::em5000 {
    enum class Posture : int64_t {
        DOWN = 0,
        STEP_1 = 1,
        STEP_2 = 2,
        STAND = 3,
        RESTRICT_DOWN = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickCustomJackControl {
    enum class ActorType : int64_t {
        UNSET = 0,
        GIMMICK = 1,
        ENEMY = 2,
        PLAYER = 3,
        NPC = 4,
        ACTOR = 5,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class UseSupportItemPhase : int64_t {
        INVOKE = 0,
        ATTACH = 1,
        DETACH = 2,
        CONSUME = 3,
        ACTION = 4,
    };
}
namespace via::render::detail {
    enum class PrimitiveShaderType : int64_t {
        GUIPoly2D = 0,
        GUIPoly2DMask = 1,
        GUIPoly2DMaskReverse = 2,
        GUIPoly2DAdd = 3,
        GUIPoly2DAddMask = 4,
        GUIPoly2DAddMaskReverse = 5,
        GUIPoly2DDetone = 6,
        GUIPoly2DMaskDetone = 7,
        GUIPoly2DMaskReverseDetone = 8,
        GUIPoly2DAddDetone = 9,
        GUIPoly2DAddMaskDetone = 10,
        GUIPoly2DAddMaskReverseDetone = 11,
        GUIGlyph2D = 12,
        GUIGlyph2DMask = 13,
        GUIGlyph2DMaskReverse = 14,
        GUIGlyph2DAdd = 15,
        GUIGlyph2DAddMask = 16,
        GUIGlyph2DAddMaskReverse = 17,
        GUIGlyph2DDetone = 18,
        GUIGlyph2DMaskDetone = 19,
        GUIGlyph2DMaskReverseDetone = 20,
        GUIGlyph2DAddDetone = 21,
        GUIGlyph2DAddMaskDetone = 22,
        GUIGlyph2DAddMaskReverseDetone = 23,
        GUIPoly3D = 24,
        GUIPoly3DTest = 25,
        GUIPoly3DMask = 26,
        GUIPoly3DMaskTest = 27,
        GUIPoly3DMaskReverse = 28,
        GUIPoly3DMaskReverseTest = 29,
        GUIPoly3DAdd = 30,
        GUIPoly3DAddTest = 31,
        GUIPoly3DAddMask = 32,
        GUIPoly3DAddMaskTest = 33,
        GUIPoly3DAddMaskReverse = 34,
        GUIPoly3DAddMaskReverseTest = 35,
        GUIPoly3DDetone = 36,
        GUIPoly3DTestDetone = 37,
        GUIPoly3DMaskDetone = 38,
        GUIPoly3DMaskTestDetone = 39,
        GUIPoly3DMaskReverseDetone = 40,
        GUIPoly3DMaskReverseTestDetone = 41,
        GUIPoly3DAddDetone = 42,
        GUIPoly3DAddTestDetone = 43,
        GUIPoly3DAddMaskDetone = 44,
        GUIPoly3DAddMaskTestDetone = 45,
        GUIPoly3DAddMaskReverseDetone = 46,
        GUIPoly3DAddMaskReverseTestDetone = 47,
        GUIGlyph3D = 48,
        GUIGlyph3DTest = 49,
        GUIGlyph3DMask = 50,
        GUIGlyph3DMaskTest = 51,
        GUIGlyph3DMaskReverse = 52,
        GUIGlyph3DMaskReverseTest = 53,
        GUIGlyph3DAdd = 54,
        GUIGlyph3DAddTest = 55,
        GUIGlyph3DAddMask = 56,
        GUIGlyph3DAddMaskTest = 57,
        GUIGlyph3DAddMaskReverse = 58,
        GUIGlyph3DAddMaskReverseTest = 59,
        GUIGlyph3DDetone = 60,
        GUIGlyph3DTestDetone = 61,
        GUIGlyph3DMaskDetone = 62,
        GUIGlyph3DMaskTestDetone = 63,
        GUIGlyph3DMaskReverseDetone = 64,
        GUIGlyph3DMaskReverseTestDetone = 65,
        GUIGlyph3DAddDetone = 66,
        GUIGlyph3DAddTestDetone = 67,
        GUIGlyph3DAddMaskDetone = 68,
        GUIGlyph3DAddMaskTestDetone = 69,
        GUIGlyph3DAddMaskReverseDetone = 70,
        GUIGlyph3DAddMaskReverseTestDetone = 71,
        GUIPolyPers = 72,
        GUIPolyPersTest = 73,
        GUIPolyPersMask = 74,
        GUIPolyPersMaskTest = 75,
        GUIPolyPersMaskReverse = 76,
        GUIPolyPersMaskReverseTest = 77,
        GUIPolyPersAdd = 78,
        GUIPolyPersAddTest = 79,
        GUIPolyPersAddMask = 80,
        GUIPolyPersAddMaskTest = 81,
        GUIPolyPersAddMaskReverse = 82,
        GUIPolyPersAddMaskReverseTest = 83,
        GUIGlyphPers = 84,
        GUIGlyphPersTest = 85,
        GUIGlyphPersMask = 86,
        GUIGlyphPersMaskTest = 87,
        GUIGlyphPersMaskReverse = 88,
        GUIGlyphPersMaskReverseTest = 89,
        GUIGlyphPersAdd = 90,
        GUIGlyphPersAddTest = 91,
        GUIGlyphPersAddMask = 92,
        GUIGlyphPersAddMaskTest = 93,
        GUIGlyphPersAddMaskReverse = 94,
        GUIGlyphPersAddMaskReverseTest = 95,
        GUIPoly2DMaskWrite = 96,
        GUIPoly3DMaskWrite = 97,
        GUIPoly3DMaskWriteTest = 98,
        GUIPolyPersMaskWrite = 99,
        GUIPolyPersMaskWriteTest = 100,
        GUIGlyph2DMaskWrite = 101,
        GUIGlyph3DMaskWrite = 102,
        GUIGlyph3DMaskWriteTest = 103,
        GUIGlyphPersMaskWrite = 104,
        GUIGlyphPersMaskWriteTest = 105,
        GUIPoly2DMaskWriteBlend2 = 106,
        GUIPoly3DMaskWriteBlend2 = 107,
        GUIPoly3DMaskWriteTestBlend2 = 108,
        GUIPolyPersMaskWriteBlend2 = 109,
        GUIPolyPersMaskWriteTestBlend2 = 110,
        GUIGlyph2DMaskWriteBlend2 = 111,
        GUIGlyph3DMaskWriteBlend2 = 112,
        GUIGlyph3DMaskWriteTestBlend2 = 113,
        GUIGlyphPersMaskWriteBlend2 = 114,
        GUIGlyphPersMaskWriteTestBlend2 = 115,
        GUIPoly2DMaskWriteBlend3 = 116,
        GUIPoly3DMaskWriteBlend3 = 117,
        GUIPoly3DMaskWriteTestBlend3 = 118,
        GUIPolyPersMaskWriteBlend3 = 119,
        GUIPolyPersMaskWriteTestBlend3 = 120,
        GUIGlyph2DMaskWriteBlend3 = 121,
        GUIGlyph3DMaskWriteBlend3 = 122,
        GUIGlyph3DMaskWriteTestBlend3 = 123,
        GUIGlyphPersMaskWriteBlend3 = 124,
        GUIGlyphPersMaskWriteTestBlend3 = 125,
        GUIBlurInstant2D = 126,
        GUIBlurInstant3D = 127,
        GUIBlurInstant3DTest = 128,
        GUIBlurGaussian2D = 129,
        GUIBlurGaussian3D = 130,
        GUIBlurGaussian3DTest = 131,
        GUIPoly2DBlendDisable = 132,
        GUIPoly3DBlendDisable = 133,
        GUIPolyPersBlendDisable = 134,
        LensflareIris = 135,
        LensflareHoop = 136,
        LensflareGeneric = 137,
        LensflareDebug = 138,
        Max = 139,
    };
}
namespace app::ropeway::environment::navigation::NavigationMapClient {
    enum class Purpose : int64_t {
        Genaral = 0,
        em3000 = 1,
        em4000 = 2,
        em6200 = 3,
        Around = 4,
        em3000_Around = 5,
    };
}
namespace app::ropeway::enemy::common::EnemyGEyeColorController::HPColorData {
    enum class eVariableType : int64_t {
        None = 0,
        Float = 1,
        Float4 = 2,
    };
}
namespace app::ropeway::enemy::em6200::fsmv2::action::Em6200FsmAction_SwitchIkArmFit {
    enum class EXEC_TYPE : int64_t {
        Start = 0,
        Update = 1,
        End = 2,
    };
}
namespace app::Collision::MotionDecalTrackRoot {
    enum class AXIS_TYPE : int64_t {
        Y_MINUS = 0,
        Y = 1,
        X_MINUS = 2,
        X = 3,
        Z_MINUS = 4,
        Z = 5,
    };
}
namespace app::ropeway::SurvivorAssignManager {
    enum class CastingOrderResult : int64_t {
        Success = 0,
        Failure = 1,
    };
}
namespace app::ropeway::enemy::em7100::fsmv2::condition::TargetDirection {
    enum class CompareType : int64_t {
        Equal = 0,
        NotEqual = 1,
        Less = 2,
        LessEq = 3,
        Greater = 4,
        GreaterEq = 5,
    };
}
namespace via::motion {
    enum class EulerOrder : int64_t {
        XYZ = 0,
        YZX = 1,
        ZXY = 2,
        ZYX = 3,
        YXZ = 4,
        XZY = 5,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class PresetBreakType : int64_t {
        PRESET_SELECTOR = 0,
        STANDING = 1,
        NOHAND_STAND = 2,
        LIMPING_L = 3,
        LIMPING_R = 4,
        KNEELING_L = 5,
        KNEELING_R = 6,
        TRAILING_L = 7,
        TRAILING_R = 8,
        CREEPING = 9,
        DHARMA = 10,
    };
}
namespace app::ropeway::survivor::fsmv2::action::SurvivorOrientAction {
    enum class Timing : int64_t {
        OnStart = 0,
        OnUpdate = 1,
        OnEnd = 2,
    };
}
namespace app::ropeway::gui::Title4Behavior {
    enum class DecidedResult : int64_t {
        DUMMY = 0,
        MAX = 1,
        CANCEL = 2,
    };
}
namespace via::gui::BlurFilter {
    enum class BlurType : int64_t {
        Instant = 0,
        System = 1,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class ExtraPartsOption : int64_t {
        PL_NECK_MEAT = 0,
        PL_ORGANS = 1,
    };
}
namespace via {
    enum class SystemServicePerformanceConfiguration : int64_t {
        Invalid = 0,
        Cpu1020MhzGpu307MhzEmc1331Mhz = 1,
        Cpu1020MhzGpu384MhzEmc1331Mhz = 2,
        Cpu1020MhzGpu768MhzEmc1600Mhz = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickDoorBase {
    enum class OpenerType : int64_t {
        NONE = 0,
        PLAYER = 1,
        ENEMY = 2,
        FLAG = 3,
    };
}
namespace app::ropeway::SurvivorDefine::Damage {
    enum class Class : int64_t {
        Unspecified = 0,
        Stagger = 256,
        Toss = 512,
        Burst = 768,
        Explosion = 1024,
        Flash = 1280,
        Shock = 1536,
        Electric = 1792,
        Fear = 2048,
        Flick = 2304,
        Shade = 2560,
        Choke = 2816,
        Other = 65280,
    };
}
namespace via {
    enum class ScreenShotReportOption : int64_t {
        NoReport = 0,
        ReportOnSuccess = 1,
        ReportOnFailure = 2,
        ReportAlways = 3,
    };
}
namespace app::ropeway::gui::MapClearingResultManager::UnitKey {
    enum class KeyType : int64_t {
        HASH = 0,
        GUID = 1,
    };
}
namespace via::motion::IkLeg {
    enum class EffectorTarget : int64_t {
        Heal = 0,
        Toe = 1,
    };
}
namespace app::ropeway::IkLegWrapper {
    enum class RayCastSkipOption : int64_t {
        None = 0,
        DIV2 = 2,
        DIV4 = 4,
    };
}
namespace app::ropeway::gui::GameOverBehavior {
    enum class Menu : int64_t {
        TO_TITLE = 0,
        LOAD = 1,
        CONTINUE = 2,
        MAX = 3,
    };
}
namespace app::ropeway::inventory {
    enum class Vital : int64_t {
        Fine = 0,
        Caution = 1,
        Danger = 2,
        Dead = 3,
    };
}
namespace app::ropeway::implement {
    enum class AidJointType : int64_t {
        None = 0,
        ExtraNarrow = 1,
        Narrow = 2,
        Wide = 3,
    };
}
namespace app::ropeway::gimmick::option::InventorySettings::Param {
    enum class UsageType : int64_t {
        REDUCE = 0,
        REMOVE = 1,
        REPLACE = 2,
    };
}
namespace via::effect::script::MathEx {
    enum class SliceType : int64_t {
        Round = 0,
        Pizza = 1,
    };
}
namespace via {
    enum class Err : int64_t {
        NoERROR_ = 0,
        BadAlloc = 1,
        NoResource = 2,
        BadCast = 3,
        NotFound = 4,
        InvalidFormat = 5,
        InvalidArgument = 6,
        InvalidOperation = 7,
        InvalidAlignment = 8,
        Overflow = 9,
        Underflow = 10,
        DivideByZero = 11,
        UnexpectedType = 12,
        Unreachable = 13,
        Nullptr = 14,
        InsufficientCapacity = 15,
        OutOfRange = 16,
        InvalidStatus = 17,
        InvalidSynchronization = 18,
        NotReady = 19,
        Busy = 20,
        NotYetImplemented = 21,
        NotSupported = 22,
        NoCanDo = 23,
        Deny = 24,
        Failed = 25,
        External = 26,
        Boost = 27,
        Critical = 28,
    };
}
namespace app::ropeway::SurvivorDefine::State {
    enum class UpperBody : int64_t {
    };
}
namespace app::ropeway::gimmick::action::GimmickPressed {
    enum class BoxType : int64_t {
        BIG = 0,
        SMALL = 1,
        MAX = 2,
    };
}
namespace via::physics::CastRayResult {
    enum class Result : int64_t {
        Success = 0,
        Failure = 1,
    };
}
namespace app::ropeway::InputSystem::StickCommandPart {
    enum class Direction : int64_t {
        NEUTRAL = 0,
        FRONT = 1,
        BACK = 2,
        LEFT = 3,
        RIGHT = 4,
    };
}
namespace via::motion::SubExJointRemapOutput {
    enum class TRS : int64_t {
        Trans = 0,
        Rot = 1,
        Scale = 2,
    };
}
namespace via::effect::script {
    enum class UpdateOrder : int64_t {
        BaseOrder = -1000,
        Provider = -999,
        ObjectEffectManager = -998,
        EnvEffectManager = -997,
        EffectParentChecker = -996,
        EffectTimelineChecker = -995,
        EPVStandardCameraFollowerLate = -994,
        EPVExpertCommon = -993,
        EPVExpertAuto = -992,
        EPVExpertDestruction = -991,
        EPVExpertExplosion = -990,
        EPVExpertFootLanding = -989,
        EPVExpertGunSmoke = -988,
        EPVExpertObjectLanding = -987,
        EPVExpertMuzzleFlash = -986,
        LoadZone = -985,
        EmitZoneGroup = -984,
        EmitZone = -983,
        CullingZoneGroup = -982,
        CullingZone = -981,
        EffectManager = -980,
        EffectContainer = -979,
    };
}
namespace app::ropeway::enemy::em5000::MotionPattern {
    enum class WalkTurn : int64_t {
        L180 = 0,
        R180 = 1,
    };
}
namespace app::ropeway::gimmick::action::AreaAccessSensor::AreaWork {
    enum class DebugDrawType : int64_t {
        EDIT = 0,
        IN_ = 1,
        OUT_ = 2,
    };
}
namespace via::gui {
    enum class FilterType : int64_t {
        None = 0,
        BlurInstant = 1,
        BlurSystem = 2,
    };
}
namespace via::render {
    enum class TexturecubeFace : int64_t {
        PositiveX = 0,
        NegativeX = 1,
        PositiveY = 2,
        NegativeY = 3,
        PositiveZ = 4,
        NegativeZ = 5,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class BodyPartsSide : int64_t {
        Center = 0,
        Left = 1,
        Right = 2,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class OneShot : int64_t {
        WINDOW_CAGE_00 = 0,
        WINDOW_CAGE_01 = 1,
        WINDOW_CAGE_02 = 2,
    };
}
namespace app::ropeway::posteffect::ColorCorrectController {
    enum class Phase : int64_t {
        Stable = 0,
        Blending = 1,
    };
}
namespace via::motion::SubExJointRemapInput {
    enum class InputType : int64_t {
        Trans = 0,
        Rot = 1,
        Scale = 2,
        Cone = 3,
    };
}
namespace app::ropeway::SweetLightController {
    enum class UseType : int64_t {
        SpotLight = 0,
        ProjectionSpotLight = 1,
    };
}
namespace via::navigation::NavigationWaypoint {
    enum class TraceTarget : int64_t {
        Waypoint = 0,
        NavMesh = 1,
        VolumeSpace = 2,
    };
}
namespace app::ropeway::enemy::em6000::MotionPattern {
    enum class Role : int64_t {
        Idle = 0,
        Floatation = 1,
        Dead = 2,
    };
}
namespace via::effect::detail::EffectExecutor {
    enum class ResourceType : int64_t {
        Handle = 0,
        Stream = 1,
        Data = 2,
    };
}
namespace app::ropeway::gui::NewInventorySlotBehavior {
    enum class GuideType : int64_t {
        NoDisp = 0,
        ItemMove = 1,
        ItemSelect = 2,
        ItemUse = 3,
        ItemCombine = 4,
        ItemExCombine = 5,
        ItemBoxIN_ = 6,
        ItemBoxOut = 7,
        ItemBoxInPos = 8,
        ItemBoxOutPos = 9,
        ShortcutMove = 10,
        CommandExchange = 11,
        CommandUse = 12,
        CommandCombine = 13,
        CommandSearch = 14,
        CommandRemove = 15,
        CommandShortcut = 16,
        CommandEquip = 17,
        CommandRemoveEquip = 18,
        CommandEquipSub = 19,
        CommandRemoveEquipSub = 20,
        CommandCustomOut = 21,
        CommandGetItem = 22,
        CommandItemBoxIN_ = 23,
        CommandItemBoxOut = 24,
        CommandItemBoxInNum = 25,
        CommandItemBoxOutNum = 26,
        Next = 27,
    };
}
namespace via {
    enum class FileAccessPriority : int64_t {
        TimeCritical = 0,
        Normal = 16383,
        Idle = 32767,
    };
}
namespace via::effect {
    enum class DevelopFlag : int64_t {
        DevelopDraw = 0,
        ZTestDraw = 1,
        CullingDraw = 2,
        EmitterRangeDraw = 3,
        EmitterShapeDraw = 4,
        TypeNodeBillboardDraw = 5,
        FadeByDraw = 6,
        FluidDebugDraw = 7,
        ProfileDraw = 8,
        ShapeOperatorDraw = 9,
        PtCollisionDraw = 10,
        DrawGpuBillboard = 11,
        EnableHaze = 12,
        EffectCollision = 13,
        Max = 14,
    };
}
namespace System::Reflection {
    enum class TypeAttributes : int64_t {
        NotPublic = 0,
        AutoLayout = 0,
        AnsiClass = 0,
        Class = 0,
        Public = 1,
        NestedPublic = 2,
        NestedPrivate = 3,
        NestedFamily = 4,
        NestedAssembly = 5,
        NestedFamANDAssem = 6,
        NestedFamORAssem = 7,
        VisibilityMask = 7,
        SequentialLayout = 8,
        ExplicitLayout = 16,
        LayoutMask = 24,
        Interface = 32,
        ClassSemanticsMask = 32,
        Abstract = 128,
        Sealed = 256,
        SpecialName = 1024,
        Import = 4096,
        Serializable = 8192,
        WindowsRuntime = 16384,
        UnicodeClass = 65536,
        AutoClass = 131072,
        CustomFormatClass = 196608,
        CustomFormatMask = 12582912,
        StringFormatMask = 196608,
        BeforeFieldInit = 1048576,
        RTSpecialName = 2048,
        HasSecurity = 262144,
        ReservedMask = 264192,
    };
}
namespace app::ropeway::gui::RogueGeneralAnnounceBehavior {
    enum class State : int64_t {
        INVALID = 0xFFFFFFFF,
        DISABLE = 0,
        INIT = 1,
        UPDATE = 2,
        DEFAULT = 3,
        FINALIZE = 4,
    };
}
namespace app::ropeway::enemy::em3000::MotionPattern {
    enum class Jump : int64_t {
        CeillingFromGround = 0,
        GroundFromCeilling = 1,
        ChaseCeillingFromGround = 2,
        WallRight = 0,
        WallLeft = 1,
    };
}
namespace via::motion::JointExMultiRemapValue::OutputData {
    enum class Interpolation : int64_t {
        Linear = 0,
        Slow = 1,
        Fast = 2,
        Smooth = 3,
    };
}
namespace app::ropeway::enemy::em7200::behaviortree::condition::Em7200BtCondition_CheckBox {
    enum class CheckType : int64_t {
        WALK = 0,
        ROTATE = 1,
        ATTACK = 2,
        LOST = 3,
    };
}
namespace app::ropeway::OptionManager {
    enum class ControllerButtonAssignmentType : int64_t {
        TYPE_A = 0,
        TYPE_B = 1,
    };
}
namespace app::ropeway::camera {
    enum class Kind : int64_t {
        Normal = 0,
        Jog = 1,
        Damage = 2,
        Hold = 3,
        Holded = 4,
        Gimmick = 5,
    };
}
namespace app::ropeway::survivor::fsmv2::action::SurvivorCharacterControllerSettingAction {
    enum class RadiusType : int64_t {
        Invalid = 0,
        Default = 1,
        Idle = 2,
        Hold = 3,
        Damage = 4,
        Holded = 5,
    };
}
namespace app::ropeway::IkLegTrackController {
    enum class FootType : int64_t {
        INVALID = 0,
        TWO_LEG = 1,
        THREE_LEG = 2,
        FOUR_LEG = 3,
    };
}
namespace app::ropeway::gui::OpenedRogueRecordsBehavior {
    enum class R_OR_R : int64_t {
        ROGUE_RECORD = 0,
        ROGUE_REWARD = 1,
    };
}
namespace via::gui::Effect {
    enum class ExternType : int64_t {
        Float = 0,
        Color = 1,
    };
}
namespace app::ropeway::enemy::tracks::Em7000ColliderTrack {
    enum class ColliderGroup_W : int64_t {
        Damage = 0,
        AimTarget = 1,
    };
}
namespace app::ropeway::gui::KeyBindBehavior {
    enum class ListMode : int64_t {
        ROOT = 0,
        KEY_INPUT = 1,
        WAIT = 2,
        Invalid = 3,
    };
}
namespace app::ropeway::timeline::TimelineFadeController {
    enum class FadeRequestType : int64_t {
        Wait = 0,
        FadeIN_ = 1,
        FadeOut = 2,
    };
}
namespace app::ropeway::gamemastering::RogueRecordManager {
    enum class RogueRecordType : int64_t {
        CLEAR_NORMAL = 0,
        CLEAR_TRAINING = 1,
        SPECIAL_CONDITION = 2,
        COUNT_UP = 3,
    };
}
namespace app::ropeway::enemy::HateTargetInfo {
    enum class FindStateType : int64_t {
        Lost = 0,
        Attention = 1,
        Finding = 2,
    };
}
namespace app::ropeway::player::tag {
    enum class LookAtAttribute : int64_t {
        EYE = 1662058075,
        FACE = 231585437,
        LIGHT = 1568811990,
        HOLD = 926659005,
        NONE = 270830317,
    };
}
namespace via::hid::VrTracker {
    enum class StartStatus : int64_t {
        Success = 0,
        NotSupported = 0xFFFFFFFF,
        AlreadyStarted = -2,
        InvalidUserIndex = -3,
        InvalidDeviceHandle = -4,
        RegisterDeviceFailed = -5,
        UserMismatched = -6,
        DeviceNotConnected = -7,
    };
}
namespace app::ropeway::enemy::em6300 {
    enum class RankParamHash : int64_t {
        ATTACK_DISTANCE = 2803158372,
    };
}
namespace app::ropeway::RagdollController {
    enum class TransformTriggerBit : int64_t {
        IMMEDIATE = 0,
        MOTION_SEQUENCE = 1,
        CONTACT_WALL = 2,
        CONTACT_RIGIDBODY = 3,
    };
}
namespace via {
    enum class ScreenShotOverlayImageIndex : int64_t {
        Index0 = 0,
        Index1 = 1,
        Index2 = 2,
        Index3 = 3,
        Index4 = 4,
        Index5 = 5,
        Index6 = 6,
        Index7 = 7,
        Index8 = 8,
        Index9 = 9,
        Max = 10,
    };
}
namespace app::ropeway::gimmick::action::GimmickElectricTransformer {
    enum class RoutineType : int64_t {
        WAIT = 0,
        OPERATE = 1,
        CORRECT = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickRaccoonFigure {
    enum class State : int64_t {
        INVALID = 0,
        ALIVE_WAIT = 1,
        ALIVE_DANCING = 2,
        ALIVE_LOST_PL = 3,
        ALIVE_NOPOWER = 4,
        BROKEN = 5,
    };
}
namespace via::gui::Material {
    enum class ParamType : int64_t {
        Unknown = 0,
        Float = 1,
        Float4 = 2,
        Color = 3,
        Texture = 4,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class FreePhase : int64_t {
        FASTEST = -11,
        Before = -10,
        Manager = -9,
        After = -8,
        LATEST = -7,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class HeadSystemPhase : int64_t {
        FASTEST = -114,
        MainSceneUpdate = -113,
        SplashScreenManager = -112,
        SceneActivateSystem = -111,
        SceneLoadSystem = -110,
        AfterSceneLoadSystem = -109,
        SceneLoadEventBroker = -108,
        SceneLoadEventReceiver = -107,
        SceneLoadEventReceiverSub = -106,
        SceneLoadEventReceiverSub2 = -105,
        CameraDrawManager = -104,
        CameraDrawController = -103,
        HitController = -102,
        HitManager = -101,
        EnemyReaction = -100,
        MainFlowManager = -99,
        GameMastering = -98,
        NetworkService = -97,
        PlayerInput = -96,
        PlayerInputUnit = -95,
        UserService = -94,
        RingBufferControll = -93,
        RingBufferManager = -92,
        LATEST = -91,
    };
}
namespace app::ropeway::enemy::em6200::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_TURN = 2,
        ACT_STANDUP = 3,
        ACT_SEARCH = 4,
        ACT_OTHER_GRAP_WAIT = 5,
        ACT_PROVOKE = 6,
        ACT_DOOR_OPEN = 7,
        ACT_LADDER_UP = 8,
        ACT_LADDER_DOWN = 9,
        ACT_WAIT_LADDER = 10,
        ATK_HOOK = 2000,
        ATK_PUNCH = 2001,
        ATK_GRAPPLE = 2002,
        ATK_HOLD = 2003,
        ATK_THROW = 2004,
        ATK_HAMMER = 2005,
        ATK_DOWN_CRUSH = 2006,
        ATK_TURN_BLOW = 2007,
        ATK_SIDE_PUNCH = 2008,
        DMG_STUN = 3000,
        DMG_KNOCKBACK = 3001,
        DMG_FLASH = 3002,
        DMG_ELECTRIC = 3003,
        DMG_ELECTRIC_FINISH = 3004,
        SPT_KNIFE = 4000,
        SPT_GRENADE = 4001,
        SPT_FLASH_GRENADE = 4002,
        DIE_NORMAL = 5000,
        SET_IDLE = 6000,
        SET_SLEEP = 6001,
        DEMO_WALL_CRASH = 7000,
        DEMO_WINDOW_CRASH = 7001,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace app::ropeway::timeline::PerformanceInfo {
    enum class EventState : int64_t {
        SETUP = 0,
        STANDBY = 1,
        PLAY = 2,
        SKIP = 3,
        PAUSE = 4,
        END = 5,
        FINISH = 6,
    };
}
namespace app::ropeway::interpolate {
    enum class InterpolateRequestType : int64_t {
        POSITION = 0,
        ROTATION = 1,
        SCALE = 2,
        TILT = 3,
        NULLOFFSET_CONST = 4,
    };
}
namespace via::render {
    enum class ShaderType : int64_t {
        GBuffer = 0,
        GBufferInstancing = 1,
        ZPrePassedGBuffer = 2,
        ZPrePassedGBufferInstancing = 3,
        Shadow = 4,
        ShadowInstancing = 5,
        Pick = 6,
        PickInstancing = 7,
        Forward = 8,
        ForwardInstancing = 9,
        DepthWrite = 10,
        DepthWriteInstancing = 11,
        ZPrePass = 12,
        ZPrePassInstancing = 13,
        Water = 14,
        WaterZ = 15,
        WaterWzOcc = 16,
        WaterPartial = 17,
        WaterZPartial = 18,
        WaterPartialWzOcc = 19,
        WaterPartialCheap = 20,
        WaterZPartialCheap = 21,
        WaterPartialCheapWzOcc = 22,
        WaterSurfaceDeferred = 23,
        WaterSurfaceForward = 24,
        WaterSurfacePreZ = 25,
        PreTransform = 26,
        PreTransformWithNorm = 27,
    };
}
namespace via::navigation::map::PathInfo {
    enum class DistanceType : int64_t {
        Raw = 0,
        Path = 1,
    };
}
namespace app::ropeway {
    enum class LegStepStatus : int64_t {
        STEP_LEFT = 0,
        STEP_RIGHT = 1,
    };
}
namespace via::render::RenderConfig {
    enum class OptionSetting : int64_t {
        OFF = 0,
        ON = 1,
        CUSTOM = 2,
    };
}
namespace app::ropeway::enemy::em3000 {
    enum class CharacterControllerType : int64_t {
        DEFAULT = 1106175613,
        JUMP = 2595772414,
        JUMP_ATTACK = 1933262719,
        CONNER_UP = 3599879432,
        CONNER_DOWN = 726343862,
    };
}
namespace app::ropeway::UserServiceManager {
    enum class Phase : int64_t {
        Wait = 0,
        Normal = 1,
    };
}
namespace via::render {
    enum class SkinWeightCount : int64_t {
        SkinWeightCount_4 = 0,
        SkinWeightCount_8 = 1,
    };
}
namespace app::ropeway::weapon::generator {
    enum class BulletPusType : int64_t {
        Damage = 0,
        Break = 1,
    };
}
namespace via::render {
    enum class PQCurve : int64_t {
        ST2084 = 0,
        PQ1000 = 1,
        PQ2000 = 2,
        Custom = 3,
    };
}
namespace via::attribute::AreaQueryCategoryAttribute {
    enum class Category : int64_t {
        Unknown = 0,
        AreaItem = 1,
        Position = 2,
        GameObject = 4,
        AreaRegion = 8,
        Any = 255,
    };
}
namespace app::ropeway::RagdollController {
    enum class RagdollStatus : int64_t {
        DEACTIVATE = 0,
        PREACTIVATE = 1,
        ACTIVATE = 2,
        RETURNING = 3,
    };
}
namespace app::ropeway::FadeController {
    enum class ProcessStep : int64_t {
        FADE_IN_IDLE = 0,
        FADE_OUT_WAIT = 1,
        FADE_OUT_IDLE = 2,
        FADE_IN_WAIT = 3,
    };
}
namespace app::ropeway::enemy::em3000::fsmv2::action::Em3000FsmAction_Caution {
    enum class Direction : int64_t {
        Front = 0,
        Right = 1,
        Left = 2,
        Back_R = 3,
        Back_L = 4,
    };
}
namespace via::dynamics::ConeTwist::Initializer {
    enum class PositionType : int64_t {
        World = 0,
        Local = 1,
    };
}
namespace app::ropeway::enemy::em6200::BehaviorTreeAction {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_TURN = 2,
        ACT_SEARCH = 4,
        ACT_PROVOKE = 6,
        ACT_OTHER_GRAP_WAIT = 5,
        ACT_WAIT_LADDER = 10,
        ATK_HOOK = 2000,
        ATK_PUNCH = 2001,
        ATK_GRAPPLE = 2002,
        ATK_HAMMER = 2005,
        ATK_TURN_BLOW = 2007,
        ATK_SIDE_PUNCH = 2008,
        NONE = 0xFFFFFFFF,
    };
}
namespace app::ropeway::survivor::SurvivorFacialController {
    enum class WwiseGameParameterId : int64_t {
        VoiceVolumeMeter = 0,
    };
}
namespace app::ropeway::gamemastering::TitleFlow {
    enum class Story : int64_t {
        LEON = 0,
        LEON_2 = 1,
        CLAIRE = 2,
        CLAIRE_2 = 3,
        THE_4TH = 4,
        TOFU = 5,
        ROGUE = 6,
        L_A = 7,
        L_B = 8,
        L_C = 9,
        L_D = 10,
    };
}
namespace app::ropeway::SceneStandbyRequester {
    enum class Type : int64_t {
        STANDBY = 0,
        RELEASE = 1,
        ACTIVATE = 2,
        DEACTIVATE = 3,
    };
}
namespace app::ropeway::gui::ItemBoxBehavior {
    enum class Mode : int64_t {
        Disable = 0,
        DisableBlackOut = 1,
        EnableDisplay = 2,
        EnableDisplayNoMove = 3,
        EnableDisplayAndControl = 4,
        EnableDisplayAndControlFree = 5,
    };
}
namespace app::ropeway::enemy::em7100::Em7100MoveActionZoneGroup {
    enum class MoveAction : int64_t {
        Jump = 0,
        Down = 1,
    };
}
namespace app::ropeway::enemy::EnemyWeaponDataUserData::WeaponValueInfoBase {
    enum class GrenadeShellType : int64_t {
        Explosion = 0,
        Blast = 1,
        ShockWave = 2,
        Bite = 3,
    };
}
namespace via::motion::JointExMultiRemapValue::OutputData {
    enum class AttrFlags : int64_t {
        None = 0,
        BasePose = 1,
    };
}
namespace app::ropeway::gui::RogueGeneralAnnounceBehavior {
    enum class SubState : int64_t {
        INVALID = 0xFFFFFFFF,
        DISABLE = 0,
        INIT = 1,
        UPDATE = 2,
        DEFAULT = 3,
        FINALIZE = 4,
    };
}
namespace via::effect::script::EffectControlAction {
    enum class TargetGameObjectModeEnum : int64_t {
        GameObjectRef = 0,
        OwnGameObject = 1,
    };
}
namespace app::ropeway::weapon::shell::ShellDefine {
    enum class InitDirection : int64_t {
        X = 0,
        Y = 1,
        Z = 2,
    };
}
namespace app::ropeway::gui::ReticleBehavior {
    enum class SexyGunState : int64_t {
        INVALID = 0,
        SEARCH = 1,
        LOCKON = 2,
        ERROR_ = 3,
        FINISH = 4,
    };
}
namespace app::ropeway::enemy::EnemyWeaponExtraReaction {
    enum class ExtraReactionKind : int64_t {
        None = 0,
        Em0000ConditionChoice = 1,
        Em0000HeadReaction = 2,
        Em0000CriticalBreakHead = 3,
    };
}
namespace app::ropeway::gimmick::action::TriggerKey {
    enum class ButtonExclusionMask : int64_t {
        NONE = 0,
        R_UP = 1,
        R_DOWN = 2,
        R_LEFT = 4,
        R_RIGHT = 8,
        R_TRIG_TOP = 16,
        R_TRIG_BOTTOM = 32,
        L_STICK_ANY = 3840,
        L_STICK_XP = 256,
        L_STICK_XM = 512,
        L_STICK_ZP = 1024,
        L_STICK_ZM = 2048,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class TailSystemPhase : int64_t {
        FASTEST = -31,
        Listener = -30,
        MotionJack = -29,
        Motion = -28,
        SecondaryMotion = -27,
        AfterMotion = -26,
        FixedMotion = -25,
        Shell = -24,
        NoticePointManager = -23,
        NoticePointController = -22,
        NoticePointSelector = -21,
        Effect = -20,
        RestrictClient = -19,
        RestrictManager = -18,
        ObjectUpdateAndDrawManager = -17,
        AfterObjectUpdateAndDrawManager = -16,
        TemporaryObjectsManager = -15,
        TailPrepareInstantiate = -14,
        TailInstantiate = -13,
        Final = -12,
        LATEST = -11,
    };
}
namespace via::motion::JointExMultiRemapValue::OutputData::JointDriverData {
    enum class InputType : int64_t {
        Trans = 0,
        Rot = 1,
        Scale = 2,
        RotRPY = 3,
        RotPYR = 4,
        ExpMap = 5,
    };
}
namespace via::physics {
    enum class ShapeType : int64_t {
        Aabb = 0,
        Sphere = 1,
        ContinuousSphere = 2,
        Capsule = 3,
        ContinuousCapsule = 4,
        Box = 5,
        Mesh = 6,
        StaticCompound = 7,
        Area = 8,
        Triangle = 9,
        SkinningMesh = 10,
        Cylinder = 11,
        Invalid = 12,
        Max = 13,
    };
}
namespace via::areamap::AreaItem {
    enum class Mode : int64_t {
        Seconds = 0,
        Frames = 1,
    };
}
namespace via::clr {
    enum class EventFlag : int64_t {
        SpecialName = 512,
        RTSpecialName = 1024,
    };
}
namespace app::ropeway::fsmv2 {
    enum class CompareType : int64_t {
        EQUALS = 0,
        NOT_EQUALS = 1,
        LESS = 2,
        LESS_EQUALS = 3,
        GREATER = 4,
        GREATER_EQUALS = 5,
    };
}
namespace app::ropeway::gimmick::action::GimmickCommonBreakable {
    enum class RoutineNo : int64_t {
        RoutineNo_Idle = 0,
        RoutineNo_Break = 1,
    };
}
namespace app::ropeway::gamemastering::MainFlowManager {
    enum class GameStartType : int64_t {
        INVALID = 0,
        NEW_GAME = 1,
        LOAD_GAME = 2,
        BACK_NEW_GAME = 3,
    };
}
namespace app::ropeway::MainSceneUpdateManager {
    enum class TailPrefabInstantiatePhase : int64_t {
        Wait = 0,
        Standby = 1,
        Active = 2,
    };
}
namespace via::fsm::action::SetBool {
    enum class Status : int64_t {
        FALSE_ = 0,
        TRUE_ = 1,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class PoseType : int64_t {
        STANDING = 0,
        LIMPING_L = 1,
        LIMPING_R = 2,
        KNEELING_L = 3,
        KNEELING_R = 4,
        TRAILING_L = 5,
        TRAILING_R = 6,
        CREEPING = 7,
    };
}
namespace via::effect::script::EffectManager {
    enum class EffectEndType : int64_t {
        Kill = 0,
        Finish = 1,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::ForceRequestActionSwitchRoot {
    enum class ExecType : int64_t {
        OnStart = 0,
        OnExit = 1,
        EndOfMotion = 2,
    };
}
namespace app::ropeway::gui::DialogBehavior {
    enum class IconState : int64_t {
        ICON_AUTOSAVE = 0,
        ICON_LOADING = 1,
        ICON_ATTENTION = 2,
        ICON_TEXT = 3,
        ICON_CONNECT = 4,
        ICON_NONE = 5,
    };
}
namespace app::ropeway::rogue::RogueSizeExpandRpTable {
    enum class ExpandType : int64_t {
        INVALID = 0xFFFFFFFF,
        INVENTORY = 0,
        ITEMBOX = 1,
    };
}
namespace app::ropeway::gamemastering::LoadGameDataFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        WAIT_GUI_LOADING = 1,
        OPEN_GUI_LOADING = 2,
        UNLOAD_SCENE_FOLDER = 3,
        SETUP_PLAYER = 4,
        LOAD_SCENE_FOLDER = 5,
        LOAD_LEVEL_FOLDER_WAIT = 6,
        LOAD_SCENE_FOLDER_WAIT = 7,
        LOAD_SCENARIO_SUB_WAIT = 8,
        FINALIZE = 9,
        FINALIZE_WAIT = 10,
        LOAD_SCENE_DECIDE_WAIT = 11,
    };
}
namespace app::ropeway::gamemastering::item::DetailCollisionBase {
    enum class TypeBits : int64_t {
        Enable = 1,
        Text = 2,
        Angle = 4,
        Motion = 8,
        Rename = 16,
        ChangeID = 32,
        ChangeText = 64,
        Etc = 128,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class CharacterControllerLayModeType : int64_t {
        LAY_HIPS = 0,
        LAY_CROTCH = 1,
    };
}
namespace app::ropeway::gui::FloorMapSt51Behavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_5_211_0 = 1,
    };
}
namespace app::ropeway::GameClock {
    enum class Kind : int64_t {
        System = 0,
        Game = 1,
        ActualPlaying = 2,
        DemoSpending = 3,
        InventorySpending = 4,
        PauseSpending = 5,
        ActualRecord = 6,
    };
}
namespace via::navigation::AIMapEffectorCore::ExtraLinkCreationInfo {
    enum class State : int64_t {
        Wait = 0,
        Link = 1,
    };
}
namespace via::motion::IkSpine {
    enum class COG_MANAGE_TYPE : int64_t {
        INTERNAL = 0,
        EXTERNAL = 1,
    };
}
namespace via::render::FFTBloom {
    enum class SceneFFTSize : int64_t {
        Fix512x512 = 0,
        Fix1024x512 = 1,
        Fix1024x1024 = 2,
        Fix2048x1024 = 3,
        Fix2048x2048 = 4,
        FollowScreen = 5,
    };
}
namespace via::clr {
    enum class TypeFlag : int64_t {
        VisibilityMask = 7,
        NotPublic = 0,
        Public = 1,
        NestedPublic = 2,
        NestedPrivate = 3,
        NestedFamily = 4,
        NestedAssembly = 5,
        NestedFamandAssem = 6,
        NestedFamorAssem = 7,
        LayoutMask = 24,
        AutoLayout = 0,
        SequentialLayout = 8,
        ExplicitLayout = 16,
        ClassSemanticsMask = 32,
        Class = 0,
        Interface = 32,
        Abstract = 128,
        Sealed = 256,
        SpecialName = 1024,
        Import = 4096,
        Serializable = 8192,
        WindowsRuntime = 16384,
        StringFormatMask = 196608,
        AnsiClass = 0,
        UnicodeClass = 65536,
        AutoClass = 131072,
        CustomFormatClass = 196608,
        CustomFormatMask = 12582912,
        BeforeFieldInit = 1048576,
        RTSpecialName = 2048,
        HasSecurity = 262144,
        ReservedMask = 264192,
        LocalHeap = 16777216,
        Finalize = 33554432,
        NativeType = 67108864,
        MarkFields = 134217728,
        NativeCtor = 268435456,
        Constracted = 536870912,
        ManagedVTable = 1073741824,
    };
}
namespace app::ropeway::enemy::em0000::Em0000SimplePartsConstructor {
    enum class ColliderIndex : int64_t {
        Head = 0,
        Body = 1,
        Hips = 2,
        L_UpperArm = 3,
        L_Forearm = 4,
        R_UpperArm = 5,
        R_Forearm = 6,
        L_Thigh = 7,
        L_ShIN_ = 8,
        R_Thigh = 9,
        R_ShIN_ = 10,
    };
}
namespace via::dynamics::gjk::EPA {
    enum class ClosestType : int64_t {
        Face = 0,
        VertexA = 1,
        VertexB = 2,
        VertexC = 3,
        EdgeA = 4,
        EdgeB = 5,
        EdgeC = 6,
        Invalid = 7,
    };
}
namespace app::ropeway::gimmick::action::RogueItemSpawnGimmick {
    enum class Condition : int64_t {
        Invalid = 0xFFFFFFFF,
        NotBroken = 0,
        Broken = 1,
    };
}
namespace via::dynamics::internal {
    enum class TriangleVoronoiId : int64_t {
        Internal = 0,
        Edge01 = 1,
        Edge20 = 2,
        V0 = 3,
        Edge12 = 4,
        V1 = 5,
        V2 = 6,
        Max = 7,
    };
}
namespace app::ropeway::fsmv2::SetColliderValidation {
    enum class SearchType : int64_t {
        TARGET_ONLY = 0,
        TARGET_CHILDREN_ONLY = 1,
        TARGET_AND_CHILDREN = 2,
    };
}
namespace via {
    enum class ScreenShotSaveResult : int64_t {
        Unknown = 0,
        Success = 1,
        AlbumUnknown = 2,
        AlbumFull = 3,
        AlbumFileCountLimit = 4,
        ShareUnknown = 5,
        ShareCanceled = 6,
        ShareFailed = 7,
    };
}
namespace app::ropeway::effect::script::RenderTargetTextureManager {
    enum class RTTSizeList : int64_t {
        Invalid = 0,
        L = 1,
        M = 2,
        S = 3,
        SS = 4,
    };
}
namespace via::detail::atomic {
    enum class TypeClassifier : int64_t {
        Default = 0,
        Integer = 1,
        Pointer = 2,
    };
}
namespace via::navigation::Navigation {
    enum class State : int64_t {
        Stop = 0,
        Navigation = 1,
        Navigaiton = 1,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class EnemyControlPhase : int64_t {
        FASTEST = -58,
        Param = -57,
        Think = -56,
        PreControl = -55,
        Control = -54,
        Sensor = -53,
        PostControl = -52,
        LATEST = -51,
    };
}
namespace via::motion::Chain {
    enum class CalculateMode : int64_t {
        Default = 0,
        Performance = 1,
        Balance = 2,
        Quality = 3,
    };
}
namespace app::ropeway::timeline::TimelineContents {
    enum class ProcessState : int64_t {
        Sleep = 0,
        Preparation = 1,
        Performance = 2,
        Settlement = 3,
    };
}
namespace app::ropeway::Em4000Define {
    enum class ActionSeType : int64_t {
        DEFAULT = 0,
        FENCE = 1,
        BUS = 2,
        CONTAINER = 3,
        IRON = 4,
    };
}
namespace via::effect::detail {
    enum class ShadowType : int64_t {
        None = 0,
        Enable = 1,
        ShadowOnly = 2,
    };
}
namespace app::ropeway::enemy::em6200::fsmv2::transition::Em6200MFsmTransitionEV_CheckLadderEnd {
    enum class HAND_TYPE : int64_t {
        Left = 0,
        Right = 1,
    };
}
namespace app::ropeway::fsmv2::enemy::em7100 {
    enum class ACTION_CONTROL : int64_t {
        NO_REACTION = 1149771438,
        NO_ADD_DAMAGE = 2480955703,
    };
}
namespace app::ropeway::IlluminationManager {
    enum class ApplyType : int64_t {
        START = 0,
        FLAG = 1,
        DEBUG = 2,
    };
}
namespace app::ropeway::posteffect::SetPostEffectCustom {
    enum class UseCameraBitFlags : int64_t {
        UsePlayerCamera = 1,
        PlayerSightCamera = 2,
        UseFixedCamera = 4,
        UseGimmickFixedCamera = 8,
        UseGimmickMotionCamera = 16,
        UseActionCamera = 32,
        UseEventCamera = 64,
        UseInventoryCamera = 128,
        UseInterpolationCamera = 256,
        UseTitleCamera = 512,
        UseGameOverCamera = 1024,
        UseResultCamera = 2048,
        UseFigureCamera = 4096,
        UseDebugCamera = 8192,
        UseDebug2PCamera = 16384,
    };
}
namespace app::ropeway::CatalogManager::CatalogContext {
    enum class Phase : int64_t {
        Initial = 0,
        CheckExist = 1,
        WaitExist = 2,
        KickLoad = 3,
        WaitLoad = 4,
        Loaded = 5,
    };
}
namespace via::effect::detail {
    enum class CullingTarget : int64_t {
        Emitter = 0,
        Particle = 1,
    };
}
namespace via::dev::net {
    enum class ConnectionState : int64_t {
        Initialize = 0,
        OpenConnection = 1,
        WaitForConnection = 2,
        HandShake_1 = 3,
        HandShake_2 = 4,
        HandShake_3 = 5,
        Connecting = 6,
        Connected = 7,
        ConnectionERROR_ = 8,
        Disconnected = 9,
        Disconnected_Recover = 10,
    };
}
namespace app::ropeway::gimmick::action::GimmickCustomJackControl::Transition {
    enum class CheckTransitionType : int64_t {
        FLAG = 1,
        TRIGGER = 2,
        EM_DAMAGE = 4,
    };
}
namespace app::ropeway::gui::FloorMapSt42Behavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_4_102_0 = 1,
        Room_4_402_0 = 2,
        Room_4_403_0 = 3,
        Room_4_404_0a = 4,
        Room_4_404_0b = 5,
        Room_4_407_0 = 6,
        Room_4_408_0 = 7,
        Room_4_409_0 = 8,
        Room_4_411_0 = 9,
        Room_4_412_0 = 10,
        Room_4_501_0 = 11,
        Room_4_502_0a = 12,
        Room_4_502_0b = 13,
        Room_4_502_0c = 14,
        Room_4_503_0 = 15,
        Room_4_504_0a = 16,
        Room_4_504_0b = 17,
        Room_4_505_0 = 18,
        Room_4_506_0 = 19,
        Room_4_507_0 = 20,
        Room_4_508_0 = 21,
    };
}
namespace app::ropeway::enemy::em5000 {
    enum class VinePartsID : int64_t {
        ID_00 = 10,
        ID_01 = 11,
        ID_02 = 12,
        ID_03 = 13,
        ID_04 = 14,
        ID_05 = 15,
        ID_06 = 16,
        ID_07 = 17,
        ID_08 = 18,
        ID_09 = 19,
        ID_10 = 20,
        ID_11 = 21,
        ID_12 = 22,
        ID_13 = 23,
        ID_14 = 24,
        ID_15 = 25,
        ID_16 = 26,
        ID_17 = 27,
        ID_18 = 28,
        ID_19 = 29,
        ID_20 = 30,
        ID_21 = 31,
        ID_22 = 32,
        ID_23 = 33,
    };
}
namespace app::ropeway::enemy::em7200::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_RUN = 2,
        ACT_TURN = 3,
        ACT_BACKSTEP = 4,
        ACT_REACTION = 5,
        ACT_ABARE_REACTION = 6,
        ACT_WALK_APPROACH_F = 7,
        ACT_BOX_GOTO_WALK = 8,
        ACT_BOX_GOTO_ROTATE = 9,
        ATK_SHORT_HIGH_L = 2000,
        ATK_SHORT_HIGH_R = 2001,
        ATK_SHORT_MID_L = 2002,
        ATK_ZERO_MID_R = 2003,
        ATK_CONTINUOUS = 2004,
        ATK_CONTINUOUS_ANGRY = 2005,
        ATK_BACK_MID_L = 2006,
        ATK_BOX_THROW = 2007,
        ATK_HOLD_START = 2008,
        ATK_GRAPPLE_KILL = 2009,
        ATK_GRAPPLE_KILLBACK = 2010,
        ATK_DASH = 2011,
        ATK_TURN180 = 2012,
        ATK_360 = 2013,
        ATK_RAGE = 2014,
        ATK_WALL_BREAK = 2015,
        DMG_BODY_WEEKNESS = 3000,
        DMG_STUN_LEG_L_EYE = 3001,
        DMG_STUN_BODY_BACK_EYE = 3002,
        DMG_STUN_SHOULDER_R_EYE = 3003,
        DMG_ANGRY_LEG_L_EYE = 3004,
        DMG_ANGRY_BODY_BACK_EYE = 3005,
        DMG_ANGRY_SHOULDER_R_EYE = 3006,
        DMG_LOW_LEG_L_EYE = 3007,
        DMG_LOW_BODY_BACK_EYE = 3008,
        DMG_LOW_SHOULDER_R_EYE = 3009,
        DMG_LOW_ANGRY_LEG_L_EYE = 3010,
        DMG_LOW_ANGRY_BODY_BACK_EYE = 3011,
        DMG_LOW_ANGRY_SHOULDER_R_EYE = 3012,
        DMG_FLASH = 3013,
        DMG_SHOCK = 3014,
        DMG_ACID = 3015,
        DMG_SPT_GRENADE = 4000,
        DMG_SPT_FLASHGRENADE = 4001,
        DMG_SPT_KNIFE = 4002,
        DIE_NORMAL = 5000,
        DIE_RIGID = 5001,
        DIE_SET = 5002,
        DIE_SPAWN_SET = 5003,
        DIE_NORMAL_ANOTHER = 5004,
        DIE_NORMAL_ROGUE = 5005,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace via {
    enum class RenderType : int64_t {
        Default = 0,
        Diffuse = 1,
        Specular = 2,
        LightHeatmap = 3,
        ShadowLightHeatmap = 4,
        DirectLight = 16,
        DiffuseLight = 17,
        Reflection = 18,
        Probe = 19,
        Path = 32,
        Albedo = 64,
        Metallic = 65,
        Roughness = 66,
        SSAO = 67,
        Translucency = 68,
        Normal = 69,
        ZDepth = 70,
        TextureMipMap = 128,
        SceneMipMap = 129,
        Lod = 130,
        StreamingTexture = 131,
        Occluder = 132,
        AlphaTest = 133,
        Wireframe = 256,
        FilledWireframe = 257,
        TelemetryHeatmap = 258,
        TransparentOverdraw = 512,
    };
}
namespace app::ropeway::leveldesign::ScenarioActivateController {
    enum class ScenarioPattern : int64_t {
        Default = 0,
        Pattern_A = 1,
        Pattern_B = 2,
        Pattern_C = 3,
        Pattern_D = 4,
        AlwaysOn = 5,
        AlwaysOff = 6,
    };
}
namespace via::render::detail {
    enum class TextureType : int64_t {
        Untyped = 0,
        IBL = 1,
        CubeMap = 2,
        LocalCubeMap = 3,
        HDR = 4,
    };
}
namespace app::ropeway::gamemastering::RichpresenceDefine {
    enum class DifficultyType : int64_t {
        Easy = 0,
        Normal = 1,
        Hard = 2,
        Invalid = 0xFFFFFFFF,
    };
}
namespace via::effect::gpgpu::BillboardCS::BillboardResource {
    enum class ComputeShaderType : int64_t {
        InitializeCS = 0,
        EmitCS = 1,
        EmitShapeOperatorCS = 2,
        EmitOnMeshContourCS = 3,
        EmitOnMeshContourCS_ColorMap = 4,
        EmitOnMeshContourCS_MaskMap = 5,
        EmitOnMeshContourCS_ColorMaskMap = 6,
        EmitOnMeshNormalContourCS = 7,
        EmitOnMeshNormalContourCS_ColorMap = 8,
        EmitOnMeshNormalContourCS_MaskMap = 9,
        EmitOnMeshNormalContourCS_ColorMaskMap = 10,
        UpdateCS = 11,
        UpdateVectorFieldCS = 12,
        UpdateSimpleVectorFieldCS = 13,
        UpdateVolumeFieldCS = 14,
        UpdateSimpleVolumeFieldCS = 15,
        UpdateMeshContourCS = 16,
        UpdateMeshContourVectorFieldCS = 17,
        UpdateMeshContourSimpleVectorFieldCS = 18,
        UpdateMeshContourVolumeFieldCS = 19,
        UpdateMeshContourSimpleVolumeFieldCS = 20,
        ParticleWarmingCS = 21,
        ParticleWarmingVectorFieldCS = 22,
        ParticleWarmingSimpleVectorFieldCS = 23,
        ParticleWarmingVolumeFieldCS = 24,
        ParticleWarmingSimpleVolumeFieldCS = 25,
        ParticleWarmingShapeOperatorCS = 26,
        ParticleWarmingVectorFieldShapeOperatorCS = 27,
        ParticleWarmingSimpleVectorFieldShapeOperatorCS = 28,
        ParticleWarmingVolumeFieldShapeOperatorCS = 29,
        ParticleWarmingSimpleVolumeFieldShapeOperatorCS = 30,
        ParticleWarmingOnMeshContourCS = 31,
        ParticleWarmingOnMeshContourCS_ColorMap = 32,
        ParticleWarmingOnMeshContourCS_MaskMap = 33,
        ParticleWarmingOnMeshContourCS_ColorMaskMap = 34,
        ParticleWarmingOnMeshContourVectorFieldCS = 35,
        ParticleWarmingOnMeshContourVectorFieldCS_ColorMap = 36,
        ParticleWarmingOnMeshContourVectorFieldCS_MaskMap = 37,
        ParticleWarmingOnMeshContourVectorFieldCS_ColorMaskMap = 38,
        ParticleWarmingOnMeshContourVolumeFieldCS = 39,
        ParticleWarmingOnMeshContourVolumeFieldCS_ColorMap = 40,
        ParticleWarmingOnMeshContourVolumeFieldCS_MaskMap = 41,
        ParticleWarmingOnMeshContourVolumeFieldCS_ColorMaskMap = 42,
        ParticleWarmingOnMeshContourSimpleVectorFieldCS = 43,
        ParticleWarmingOnMeshContourSimpleVectorFieldCS_ColorMap = 44,
        ParticleWarmingOnMeshContourSimpleVectorFieldCS_MaskMap = 45,
        ParticleWarmingOnMeshContourSimpleVectorFieldCS_ColorMaskMap = 46,
        ParticleWarmingOnMeshContourSimpleVolumeFieldCS = 47,
        ParticleWarmingOnMeshContourSimpleVolumeFieldCS_ColorMap = 48,
        ParticleWarmingOnMeshContourSimpleVolumeFieldCS_MaskMap = 49,
        ParticleWarmingOnMeshContourSimpleVolumeFieldCS_ColorMaskMap = 50,
        ParticleWarmingOnMeshNormalContourCS = 51,
        ParticleWarmingOnMeshNormalContourCS_ColorMap = 52,
        ParticleWarmingOnMeshNormalContourCS_MaskMap = 53,
        ParticleWarmingOnMeshNormalContourCS_ColorMaskMap = 54,
        ParticleWarmingOnMeshNormalContourVectorFieldCS = 55,
        ParticleWarmingOnMeshNormalContourVectorFieldCS_ColorMap = 56,
        ParticleWarmingOnMeshNormalContourVectorFieldCS_MaskMap = 57,
        ParticleWarmingOnMeshNormalContourVectorFieldCS_ColorMaskMap = 58,
        ParticleWarmingOnMeshNormalContourVolumeFieldCS = 59,
        ParticleWarmingOnMeshNormalContourVolumeFieldCS_ColorMap = 60,
        ParticleWarmingOnMeshNormalContourVolumeFieldCS_MaskMap = 61,
        ParticleWarmingOnMeshNormalContourVolumeFieldCS_ColorMaskMap = 62,
        ParticleWarmingOnMeshNormalContourSimpleVectorFieldCS = 63,
        ParticleWarmingOnMeshNormalContourSimpleVectorFieldCS_ColorMap = 64,
        ParticleWarmingOnMeshNormalContourSimpleVectorFieldCS_MaskMap = 65,
        ParticleWarmingOnMeshNormalContourSimpleVectorFieldCS_ColorMaskMap = 66,
        ParticleWarmingOnMeshNormalContourSimpleVolumeFieldCS = 67,
        ParticleWarmingOnMeshNormalContourSimpleVolumeFieldCS_ColorMap = 68,
        ParticleWarmingOnMeshNormalContourSimpleVolumeFieldCS_MaskMap = 69,
        ParticleWarmingOnMeshNormalContourSimpleVolumeFieldCS_ColorMaskMap = 70,
        LightingCS = 71,
        ParticleCollider = 72,
        MaxType = 73,
    };
}
namespace via::effect::detail {
    enum class PtLifeStatus : int64_t {
        Initialize = 0,
        Appear = 1,
        Keep = 2,
        Vanish = 3,
        Terminate = 4,
        Unknown = 0xFFFFFFFF,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class Em0000DirtyPresetParts : int64_t {
        Body = 0,
        Face = 1,
        Pants = 2,
        Shirt = 3,
    };
}
namespace app::ropeway::gui::ReNetDetailBehavior {
    enum class DecidedResult : int64_t {
        CLOSE = 0,
        POLICY = 1,
        MAX = 2,
        CANCEL = 3,
    };
}
namespace app::ropeway::SurvivorAssignManager {
    enum class TransactionOption : int64_t {
        None = 0,
        KeepPlayerPosition = 1,
    };
}
namespace System::Runtime::CompilerServices {
    enum class MethodCodeType : int64_t {
        IL = 0,
        Native = 1,
        OPTIL = 2,
        Runtime = 3,
    };
}
namespace app::assets::SectionRoot::Script::Character::Enemy::Em0000::Behavior::Em0000IntestineChainStarter {
    enum class StatusType : int64_t {
        Setup = 0,
        Active = 1,
        Sleep = 2,
    };
}
namespace app::ropeway::EnemyLoiteringController {
    enum class STATE : int64_t {
        Stop = 0,
        Relocation_Wait = 1,
        Loitering = 2,
        Around = 3,
    };
}
namespace app::ropeway::gamemastering::ResultExtraFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        WAIT_GUI_CREATION = 1,
        UPDATE = 2,
        WAIT_FADE = 3,
        FINALIZE = 4,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace via::gui::GUIUtility {
    enum class AssociationAnlyzeContext : int64_t {
        SearchEqual = 0,
        SearchDoubleQuot = 1,
    };
}
namespace app::ropeway::InputDefine {
    enum class Shortcut : int64_t {
        UP = 0,
        DOWN = 1,
        LEFT = 2,
        RIGHT = 3,
    };
}
namespace app::ropeway::enemy::em7200::Em7200Think {
    enum class EyeParts : int64_t {
        SHOULDER = 0,
        LEG = 1,
        BACK = 2,
    };
}
namespace app::ropeway::enemy::em7200::BehaviorTreeAction {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_RUN = 2,
        ACT_TURN = 3,
        ACT_BACKSTEP = 4,
        ACT_REACTION = 5,
        ACT_ABARE_REACTION = 6,
        ACT_WALK_APPROACH_F = 7,
        ACT_BOX_GOTO_WALK = 8,
        ACT_BOX_GOTO_ROTATE = 9,
        ATK_SHORT_HIGH_L = 2000,
        ATK_SHORT_HIGH_R = 2001,
        ATK_SHORT_MID_L = 2002,
        ATK_ZERO_MID_R = 2003,
        ATK_CONTINUOUS = 2004,
        ATK_CONTINUOUS_ANGRY = 2005,
        ATK_BACK_MID_L = 2006,
        ATK_BOX_THROW = 2007,
        ATK_HOLD_START = 2008,
        ATK_GRAPPLE_KILL = 2009,
        ATK_GRAPPLE_KILLBACK = 2010,
        ATK_DASH = 2011,
        ATK_TURN180 = 2012,
        ATK_360 = 2013,
        ATK_RAGE = 2014,
    };
}
namespace via::storage::saveService {
    enum class SaveServiceMode : int64_t {
        SaveServiceMode_Default = 131072,
        SaveServiceMode_1_Byte = 1,
        SaveServiceMode_512_Sector = 512,
        SaveServiceMode_1_KiloByte = 1024,
        SaveServiceMode_4K_Sector = 4096,
        SaveServiceMode_64_KiloByte = 65536,
        SaveServiceMode_128_KiloByte = 131072,
        SaveServiceMode_256_KiloByte = 262144,
        SaveServiceMode_512_KiloByte = 524288,
        SaveServiceMode_1_MegaByte = 1048576,
        SaveServiceMode_HighSpeed = 2147483647,
    };
}
namespace app::ropeway::gimmick::action::RogueItemSpawnGimmick {
    enum class SetSituationType : int64_t {
        Invalid = 0xFFFFFFFF,
        None = 0,
        Ceiling = 1,
        Wall = 2,
        Floor = 3,
    };
}
namespace via::motion::IkLeg {
    enum class CenterAdjust : int64_t {
        None = 0,
        ToRoot = 1,
        Center = 2,
    };
}
namespace via::gui {
    enum class MessageType : int64_t {
        Dynamic = 0,
        Static = 1,
    };
}
namespace app::ropeway::DebugSetModel {
    enum class DebugSetModelType : int64_t {
        INVALID = 0,
        ITEM = 1,
        MEMO = 2,
    };
}
namespace app::ropeway::enemy::em4000::Em4000Gimmick {
    enum class FlagCheckType : int64_t {
        TRUE_ = 0,
        FALSE_ = 1,
    };
}
namespace app::ropeway::gui::FloorMapSt43Behavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        Door_2_1_201 = 1,
        Door_2_1_202 = 2,
        Door_2_1_204 = 3,
        Door_2_1_205 = 4,
        Door_2_1_206 = 5,
        Door_2_1_208 = 6,
        Door_2_1_209 = 7,
        Door_2_1_210 = 8,
        Door_2_1_211 = 9,
        Door_2_1_213 = 10,
    };
}
namespace via::hid {
    enum class PauseGroup_hdVibration : int64_t {
        PauseGroup_00 = 0,
        PauseGroup_01 = 1,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class RequestSetId : int64_t {
        Press = 0,
        GimmickSensor = 1,
        TouchApeal = 2,
        SoundSensor = 3,
    };
}
namespace via::AABB {
    enum class VoronoiId : int64_t {
        X_MinBit = 1,
        X_MaxBit = 2,
        Y_MinBit = 4,
        Y_MaxBit = 8,
        Z_MinBit = 16,
        Z_MaxBit = 32,
        Internal = 0,
        P_YZX0 = 1,
        P_YZX1 = 2,
        P_ZXY0 = 4,
        P_ZXY1 = 8,
        P_XYZ0 = 16,
        P_XYZ1 = 32,
        E_XY0Z0 = 20,
        E_XY1Z0 = 24,
        E_XY0Z1 = 36,
        E_XY1Z1 = 40,
        E_YZ0X0 = 17,
        E_YZ1X0 = 33,
        E_YZ0X1 = 18,
        E_YZ1X1 = 34,
        E_ZX0Y0 = 5,
        E_ZX1Y0 = 6,
        E_ZX0Y1 = 9,
        E_ZX1Y1 = 10,
        V_X0Y0Z0 = 21,
        V_X1Y0Z0 = 22,
        V_X0Y1Z0 = 25,
        V_X1Y1Z0 = 26,
        V_X0Y0Z1 = 37,
        V_X1Y0Z1 = 38,
        V_X0Y1Z1 = 41,
        V_X1Y1Z1 = 42,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class HandsType : int64_t {
        BOTH_HANDS = 0,
        L_HAND_ONLY = 1,
        R_HAND_ONLY = 2,
        BOTH_ELBOW = 3,
        L_ELBOW_ONLY = 4,
        R_ELBOW_ONLY = 5,
        BOTH_UPPER = 6,
        L_UPPER_ONLY = 7,
        R_UPPER_ONLY = 8,
        BOTH_SHOULDER = 9,
        L_SHOULDER_ONLY = 10,
        R_SHOULDER_ONLY = 11,
        LOST_ARMS = 12,
    };
}
namespace app::ropeway::enemy::em6000::MotionPattern {
    enum class WalkStart : int64_t {
        Front = 0,
        FrontL = 1,
        FrontR = 2,
        BackL = 3,
        BackR = 4,
    };
}
namespace app::ropeway::gui::RogueInventorySlotBehavior {
    enum class ExCombinationStockType : int64_t {
        Invalid = 0xFFFFFFFF,
        OneToOne = 0,
        OneToFat = 1,
        FatToOne = 2,
    };
}
namespace via::clr {
    enum class MethodSectType : int64_t {
        EHTable = 1,
        OptILTable = 2,
        FatFormat = 64,
        MoreSects = 128,
    };
}
namespace app::ropeway::SurvivorDefine::Damage {
    enum class ActionLayer : int64_t {
        Unspecified = 0,
        TotalBody = 16777216,
        UpperBody = 33554432,
    };
}
namespace app::ropeway::gimmick::option::TextMessageSettings {
    enum class ResultType : int64_t {
        NotYet = 0,
        Success = 1,
        Failed = 2,
    };
}
namespace app::ropeway::network::service::StoreServiceControllerForXB1 {
    enum class ProductItemTypes : int64_t {
        App = 1,
        Consumable = 4,
        Durable = 8,
        Game = 1,
        GameDemo = 16,
    };
}
namespace app::ropeway::gui::NewInventorySlotBehavior {
    enum class SlotSelectMode : int64_t {
        Normal = 0,
        Combination = 1,
        ExchangeCombination = 2,
        GetItem = 3,
        UseItem = 4,
        ItemBox = 5,
        Shortcut = 6,
        Detail = 7,
    };
}
namespace app::ropeway::enemy::em7100 {
    enum class AddDamageType : int64_t {
        ADD_DAMAGE_TYPE_HEAD = 0,
        ADD_DAMAGE_TYPE_ARM_L = 1,
        ADD_DAMAGE_TYPE_ARM_R = 2,
        ADD_DAMAGE_TYPE_CHEST_LF = 3,
        ADD_DAMAGE_TYPE_CHEST_LB = 4,
        ADD_DAMAGE_TYPE_CHEST_RF = 5,
        ADD_DAMAGE_TYPE_CHEST_RB = 6,
        ADD_DAMAGE_TYPE_STOMACH_F = 7,
        ADD_DAMAGE_TYPE_STOMACH_B = 8,
        ADD_DAMAGE_TYPE_LEG_L = 9,
        ADD_DAMAGE_TYPE_LEG_R = 10,
    };
}
namespace app::ropeway::enemy::em3000::fsmv2::action::Em3000FsmAction_SetAttackData {
    enum class AttackKind : int64_t {
        CLAW = 0,
        CLAW_TWO = 1,
        CLAW_FLASH = 2,
        CLAW_TWO_FLASH = 3,
        JUMP_CLAW = 4,
        STICK_CLAW = 5,
        TONGUE = 6,
    };
}
namespace app::ropeway::gui::RogueItemBoxBehavior {
    enum class SubMode : int64_t {
        Default = 0,
        RuckSack = 1,
        ExCombination = 2,
    };
}
namespace via::effect::detail {
    enum class EmitterFlags : int64_t {
        Root = 1,
        Ungrouped = 2,
    };
}
namespace via::render {
    enum class AtmosphereSimulationPreset : int64_t {
        Custom = 0,
        Noon = 1,
        Sunset = 2,
        NightWithSunAsMoon = 3,
    };
}
namespace via::storage::saveService {
    enum class SaveTransferring : int64_t {
        Default = 0,
        Setting_1 = 1,
        Setting_2 = 2,
        Setting_3 = 3,
        Setting_4 = 4,
        Setting_5 = 5,
        Setting_6 = 6,
        Setting_7 = 7,
        Setting_8 = 8,
        Setting_9 = 9,
        Setting_10 = 10,
        Setting_11 = 11,
        Setting_12 = 12,
        Setting_13 = 13,
        Setting_14 = 14,
        Setting_15 = 15,
        Setting_16 = 16,
        Setting_17 = 17,
        Setting_18 = 18,
        Setting_19 = 19,
        Setting_20 = 20,
        Setting_21 = 21,
        Setting_22 = 22,
        Setting_23 = 23,
        Setting_24 = 24,
        Setting_25 = 25,
        Setting_26 = 26,
        Setting_27 = 27,
        Setting_28 = 28,
        Setting_29 = 29,
        Setting_30 = 30,
        Setting_31 = 31,
        Setting_32 = 32,
        Setting_33 = 33,
        Setting_34 = 34,
        Setting_35 = 35,
        Setting_36 = 36,
        Setting_37 = 37,
        Setting_38 = 38,
        Setting_39 = 39,
        Setting_40 = 40,
        Setting_41 = 41,
        Setting_42 = 42,
        Setting_43 = 43,
        Setting_44 = 44,
        Setting_45 = 45,
        Setting_46 = 46,
        Setting_47 = 47,
        Setting_48 = 48,
        Setting_49 = 49,
        Setting_50 = 50,
        Setting_51 = 51,
        Setting_52 = 52,
        Setting_53 = 53,
        Setting_54 = 54,
        Setting_55 = 55,
        Setting_56 = 56,
        Setting_57 = 57,
        Setting_58 = 58,
        Setting_59 = 59,
        Setting_60 = 60,
        Setting_61 = 61,
        Setting_62 = 62,
        Setting_63 = 63,
        Setting_64 = 64,
        Setting_65 = 65,
        Setting_66 = 66,
        Setting_67 = 67,
        Setting_68 = 68,
        Setting_69 = 69,
        Setting_70 = 70,
        Setting_71 = 71,
        Setting_72 = 72,
        Setting_73 = 73,
        Setting_74 = 74,
        Setting_75 = 75,
        Setting_76 = 76,
        Setting_77 = 77,
        Setting_78 = 78,
        Setting_79 = 79,
        Setting_80 = 80,
        Setting_81 = 81,
        Setting_82 = 82,
        Setting_83 = 83,
        Setting_84 = 84,
        Setting_85 = 85,
        Setting_86 = 86,
        Setting_87 = 87,
        Setting_88 = 88,
        Setting_89 = 89,
        Setting_90 = 90,
        Setting_91 = 91,
        Setting_92 = 92,
        Setting_93 = 93,
        Setting_94 = 94,
        Setting_95 = 95,
        Setting_96 = 96,
        Setting_97 = 97,
        Setting_98 = 98,
        Setting_99 = 99,
        TransferringMax = 100,
    };
}
namespace app::ropeway::enemy::common::fsmv2::condition::LastDamageDirection {
    enum class ForwardDir : int64_t {
        Front = 0,
        Back = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickCustomJackControl {
    enum class InteractLeverType : int64_t {
        NONE = 0,
        ZP = 1,
        ZM = 2,
        XP = 3,
        XM = 4,
    };
}
namespace via::effect::detail {
    enum class RotateAnimFlagType : int64_t {
        EnableReverseRandom = 1,
    };
}
namespace app::ropeway::gui::FloorMapSt1CBehavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        Door_5_3_001 = 1,
    };
}
namespace app::ropeway::gui::MenuMainBehavior {
    enum class DecidedResult : int64_t {
        STORY = 0,
        EXTRA = 1,
        BONUS = 2,
        ONLINE_STORE = 3,
        OPTIONS = 4,
        EXIT = 5,
        MAX = 6,
        CANCEL = 7,
    };
}
namespace app::ropeway {
    enum class IkKind : int64_t {
        LEG = 0,
        SPINE = 1,
        LOOKAT = 2,
        ARM = 3,
        ARMFIT = 4,
        HAND = 5,
    };
}
namespace via::nnfc::nfp {
    enum class NfpCfgBit : int64_t {
        UseDialogOnCommonERROR_ = 1,
        AutoRunApplet = 2,
        Default = 3,
    };
}
namespace app::ropeway::gui::RogueInventoryBehavior {
    enum class CallOpenSubMode : int64_t {
        Default = 0,
        RuckSack = 1,
        InGameShop = 2,
    };
}
namespace app::ropeway::NPCAccessManager::RequestResult {
    enum class ResultType : int64_t {
        UNSET = 0,
        REQUEST = 1,
        ANYONE_USED = 2,
    };
}
namespace app::ropeway::inventory {
    enum class SlotType : int64_t {
        Invalid = 0xFFFFFFFF,
        Blank = 0,
        Item = 1,
        Weapon = 2,
        Dead = 3,
    };
}
namespace app::ropeway::gimmick::action::DevelopInteractCustom {
    enum class DevelopType : int64_t {
        NONE = 0,
        FORCE_ENABLE_ON_BOX = 1,
    };
}
namespace app::ropeway::enemy::em4100::behaviortree::condition::Em4100BtCondition_CheckSetType {
    enum class ExpressionType : int64_t {
        EQ = 0,
        NE = 1,
    };
}
namespace via::gui {
    enum class MouseEventType : int64_t {
        Unknown = 0,
        Enter = 1,
        Over = 2,
        Leave = 3,
    };
}
namespace via::network::bson {
    enum class ElementId : int64_t {
        EOO = 0,
        Double = 1,
        String = 2,
        Object = 3,
        Array = 4,
        Binary = 5,
        Bool = 8,
        Int32 = 16,
        Timestamp = 17,
        Int64 = 18,
    };
}
namespace app::ropeway::MainSceneUpdateManager {
    enum class ActivateMode : int64_t {
        FrameHead = 0,
        Immediately = 1,
    };
}
namespace via::motion::JointRemapValue::RemapValueItem {
    enum class Axis : int64_t {
        X = 0,
        Y = 1,
        Z = 2,
    };
}
namespace via::charset {
    enum class EncodingType : int64_t {
        Unknown = 0,
        Ascii = 1,
        ShiftJIS = 2,
        UTF8_BOM = 3,
        UTF8 = 4,
    };
}
namespace via::network::wrangler {
    enum class EventEnabledState : int64_t {
        Undefined = 0,
        Off = 1,
        ProviderDefault = 2,
        On = 3,
    };
}
namespace app::ropeway::enemy::em0000::RoleAction {
    enum class KnockSet : int64_t {
        WAIT = 0,
        LOOP_START = 1,
        WAIT_LOOP_START = 2,
    };
}
namespace via::physics::CharacterController {
    enum class State : int64_t {
        FirstUpdate = 0,
        OverwritePosition = 1,
        IgnoreVerticalRiseAdjust = 2,
        UpdateFilterInfo = 3,
        AfterUnloaded = 4,
        Max = 5,
    };
}
namespace app::ropeway::enemy::em7200::Em7200Think {
    enum class ThrowDirType : int64_t {
        FRONT = 0,
        BACK = 1,
        LEFT = 2,
        RIGHT = 3,
        NEAR_LEFT = 4,
        NEAR_RIGHT = 5,
        LARGE_STRIKE = 6,
    };
}
namespace via::render {
    enum class OutputLimitConfigMode : int64_t {
        Disable = 0,
        LowerLimit = 1,
        UpperLimit = 2,
        UserImage = 3,
    };
}
namespace via::wwise {
    enum class SinkTypeXboxOne : int64_t {
        MaIN_ = 0,
        MergeToMaIN_ = 1,
        BGM = 2,
        Communication = 3,
        Personal = 4,
        None = 5,
    };
}
namespace app::ropeway::RogueEnemySpawnManager::DeadVanishRequest {
    enum class VanishState : int64_t {
        Wait = 0,
        Ready = 1,
        Vanishing = 2,
        Destroying = 3,
        Finished = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickBreakableColumnForG2Battle {
    enum class HoldType : int64_t {
        Hold = 0,
        Through = 1,
    };
}
namespace via::gui::MaterialText {
    enum class ParamType : int64_t {
        Unknown = 0,
        Float = 1,
        Float4 = 2,
        Color = 3,
        Texture = 4,
    };
}
namespace via::motion::tree::SelectMotionNode {
    enum class BankSelect : int64_t {
        Base = 0,
        Tbl = 1,
        Direct = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickC4Bomb {
    enum class C4State : int64_t {
        INIT = 0,
        WAIT = 1,
        COUNTING = 2,
        BOMB = 3,
        DISABLE = 4,
    };
}
namespace app::ropeway::enemy::em6200::fsmv2::stateaction::Em6200FsmStateAction_CheckDoorOverTurn {
    enum class CheckRoutine : int64_t {
        Request = 0,
        CalcWait = 1,
        Derivation = 2,
    };
}
namespace via::render {
    enum class SamplerQuality : int64_t {
        Bilinear = 0,
        Trilinear = 1,
        Anisotropic2 = 2,
        Anisotropic4 = 3,
        Anisotropic8 = 4,
        Anisotropic16 = 5,
    };
}
namespace app::ropeway::fsmv2::enemy::action::ObstacleControllerSwitch {
    enum class SwitchPattern : int64_t {
        TurnOn = 0,
        TurnOff = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickRogueLevelUp {
    enum class ArgNo : int64_t {
        Open = 0,
        Wait = 1,
    };
}
namespace app::ropeway::CameraTwirler {
    enum class LockType : int64_t {
        FREE = 0,
        PITCH = 1,
        YAW = 2,
        BOTH = 3,
    };
}
namespace via::navigation {
    enum class GraphDrawMode : int64_t {
        Normal = 0,
        Light = 1,
    };
}
namespace app::ropeway::timeline::TimelineEventSettingTable {
    enum class ApplyTiming : int64_t {
        PreStart = 0,
        Cleanup = 1,
    };
}
namespace app::ropeway::fsmv2::GmkCrocodileCamera {
    enum class CameraState : int64_t {
        START = 0,
        STOP = 1,
        KILL = 2,
        BREAKKILL = 3,
    };
}
namespace app::ropeway::enemy::em6000 {
    enum class RangeID : int64_t {
        SNear = 0,
        Near = 1,
        Middle = 2,
        Far = 3,
        SFar = 4,
        NUM = 5,
    };
}
namespace app::ropeway::gimmick::action::RestageEventFinished {
    enum class FinishCheckType : int64_t {
        FLAG = 0,
        FIRER = 1,
    };
}
namespace app::ropeway::EnvironmentStandbyManager {
    enum class PauseLoadReason : int64_t {
        None = 0,
        Timeline = 1,
        SurvivorControl = 2,
        DebugMenu = 4,
    };
}
namespace app::ropeway::enemy::em5000::Em5000Think {
    enum class CheckEliminateResult : int64_t {
        Stay = 0,
        RestrictRevival = 1,
        Eliminate = 2,
    };
}
namespace app::ropeway {
    enum class PressGroupID : int64_t {
        Group_00 = 0,
        Group_01 = 1,
        Group_02 = 2,
        Group_03 = 3,
        Group_04 = 4,
        Group_05 = 5,
        Group_06 = 6,
        Group_07 = 7,
    };
}
namespace via::render {
    enum class TonemapFunction : int64_t {
        Karis = 0,
        TripleSection = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickCustomJackControl::ActorWorkPlayer {
    enum class SetPosType : int64_t {
        INTERP_POS = 0,
        INTERP_ROT = 1,
        INTERP_POSROT = 2,
        INTERP_BY_NULLOFS = 3,
        NONE = 4,
    };
}
namespace app::ropeway::IkLegTrackController {
    enum class FootBits : int64_t {
        LEFT = 1,
        RIGHT = 2,
        FRONT_LEFT = 4,
        FRONT_RIGHT = 8,
    };
}
namespace via::render::detail {
    enum class PrimCmdType : int64_t {
        Unknown = 0,
        Polygon = 1,
        Mesh = 2,
        MeshInstancing = 3,
    };
}
namespace via::render {
    enum class DecalReciveMode : int64_t {
        Automatic = 0,
        Enable = 1,
        Disable = 2,
    };
}
namespace app::ropeway::EnvironmentStandbyManager {
    enum class LocationLoadReason : int64_t {
        None = 0,
        ForceUnload = 1,
        NEED_TO_UNLOAD = 2,
        Keep = 3,
        NEED_TO_ACTIVE = 4,
        Myself = 5,
        LoadCollision = 6,
        ManagerRequest = 7,
        DirectRequest = 8,
        OpeningLoadExpress = 9,
    };
}
namespace via::clr {
    enum class SignatureType : int64_t {
        Default = 0,
        C = 1,
        StdCall = 2,
        ThisCall = 3,
        FastCall = 4,
        VarArg = 5,
        Field = 6,
        LocalVar = 7,
        Property = 8,
        TypeSpec = 9,
        MethodSpec = 10,
    };
}
namespace app::ropeway::JointDefine {
    enum class OffsetType : int64_t {
        Joint = 0,
        Object = 1,
        Camera = 2,
        CameraY = 3,
    };
}
namespace app::ropeway::gamemastering::ForceTitleFlowVS {
    enum class State : int64_t {
        INITIALIZE = 0,
        UPDATE = 1,
        DIALOG = 2,
        FINALIZE = 3,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace via::clr {
    enum class EvalType : int64_t {
        Int32 = 0,
        Int64 = 1,
        Float = 2,
        Ptr = 3,
        ObjRef = 4,
        Value = 5,
        Invalid = 7,
    };
}
namespace app::ropeway::DamageInfoBase {
    enum class Direction : int64_t {
        Front = 0,
        Back = 1,
        Left = 2,
        Right = 3,
    };
}
namespace via::wwise {
    enum class StateThread : int64_t {
        None = 0,
        Uninitialized = 1,
        Initializing = 2,
        Idle = 3,
        Executing = 4,
        Terminated = 5,
        Max = 6,
    };
}
namespace app::ropeway::enemy::em4400 {
    enum class LIVE_AREA_OVER_TYPE : int64_t {
        PUSH_BACK = 0,
        HIDE = 1,
    };
}
namespace via::charset {
    enum class UTF16Type : int64_t {
        LE = 0,
        BE = 1,
        LE_BOM = 2,
        BE_BOM = 3,
        Native = 0,
    };
}
namespace app::ropeway::survivor::npc::NpcNoticePointSelector {
    enum class SettingType : int64_t {
        Default = 0,
        Attention = 1,
        Battle_VeryFar = 2,
    };
}
namespace via::os {
    enum class SemaphoreError : int64_t {
        None = 0,
        Failed = 1,
    };
}
namespace app::ropeway::gui::OpenedRecordsBehavior {
    enum class R_OR_R : int64_t {
        RECORD = 0,
        REWARD = 1,
    };
}
namespace app::ropeway::EnemyGimmickConfiscateController {
    enum class WINDOW_CONDITION : int64_t {
        IsHitable = 0,
        IsBreakable = 1,
        IsInvadable = 2,
        HasBarricade = 3,
        IsBroken = 4,
        IsFired = 5,
    };
}
namespace via::effect::detail {
    enum class PlayType : int64_t {
        Pause = 0,
        Loop = 1,
        Finish = 2,
        Play = 3,
    };
}
namespace app::ropeway::fsmv2::GmkHieroglyphicDialAction {
    enum class StateType : int64_t {
        ANIM_START = 0,
        ANIM_END = 1,
    };
}
namespace app::ropeway::gui::TitleTBehavior {
    enum class DecidedResult : int64_t {
        DUMMY = 0,
        MAX = 1,
        CANCEL = 2,
    };
}
namespace via::gui {
    enum class RenderLayerType : int64_t {
        Overlay = 0,
        Transparent = 1,
    };
}
namespace app::ropeway::gui::RogueInventoryBehavior {
    enum class CallOpenMode : int64_t {
        Normal = 0,
        Map = 1,
        GetMap = 2,
        Map4th = 3,
        GetItem = 4,
        GetItemShortcut = 5,
        UseItem = 6,
        ItemBox = 7,
    };
}
namespace app::ropeway::enemy::tracks::Em7100DecalTrack {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Press = 0,
        Damage = 1,
        Sensor_Touch = 2,
        Attack = 3,
        Attack_Burst = 4,
        ToPiller_Hold = 5,
        ToPiller_Break = 6,
        Attack_Head = 7,
        Attack_L_Arm = 8,
        Attack_Grapple_Kill = 9,
        MarkerSensor = 10,
        Decal = 11,
    };
}
namespace via::effect::script::EffectCommonDefine {
    enum class EffectActionOnProviderDestroy : int64_t {
        None = 0,
        Finish = 1,
        Kill = 2,
    };
}
namespace via::hid {
    enum class KeyboardKey : int64_t {
        None = 0,
        LButton = 1,
        RButton = 2,
        Cancel = 3,
        MButton = 4,
        XButton1 = 5,
        XButton2 = 6,
        Back = 8,
        Tab = 9,
        Clear = 12,
        Enter = 13,
        Return = 13,
        Shift = 16,
        Control = 17,
        Menu = 18,
        Pause = 19,
        Capital = 20,
        Kana = 21,
        Junja = 23,
        Final = 24,
        Hanja = 25,
        Escape = 27,
        Convert = 28,
        NonConvert = 29,
        Accept = 30,
        ModeChange = 31,
        Space = 32,
        Prior = 33,
        Next = 34,
        End = 35,
        Home = 36,
        Left = 37,
        Up = 38,
        Right = 39,
        Down = 40,
        Select = 41,
        Print = 42,
        Execute = 43,
        SnapShot = 44,
        Insert = 45,
        Delete = 46,
        Help = 47,
        Alpha0 = 48,
        Alpha1 = 49,
        Alpha2 = 50,
        Alpha3 = 51,
        Alpha4 = 52,
        Alpha5 = 53,
        Alpha6 = 54,
        Alpha7 = 55,
        Alpha8 = 56,
        Alpha9 = 57,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LWIN_ = 91,
        RWIN_ = 92,
        Apps = 93,
        Sleep = 95,
        NumPad0 = 96,
        NumPad1 = 97,
        NumPad2 = 98,
        NumPad3 = 99,
        NumPad4 = 100,
        NumPad5 = 101,
        NumPad6 = 102,
        NumPad7 = 103,
        NumPad8 = 104,
        NumPad9 = 105,
        Multiply = 106,
        Add = 107,
        Separator = 108,
        Subtract = 109,
        Decimal = 110,
        Divide = 111,
        F1 = 112,
        F2 = 113,
        F3 = 114,
        F4 = 115,
        F5 = 116,
        F6 = 117,
        F7 = 118,
        F8 = 119,
        F9 = 120,
        F10 = 121,
        F11 = 122,
        F12 = 123,
        F13 = 124,
        F14 = 125,
        F15 = 126,
        F16 = 127,
        F17 = 128,
        F18 = 129,
        F19 = 130,
        F20 = 131,
        F21 = 132,
        F22 = 133,
        F23 = 134,
        F24 = 135,
        NumLock = 144,
        Scroll = 145,
        NumPadEnter = 146,
        LShift = 160,
        RShift = 161,
        LControl = 162,
        RControl = 163,
        LMenu = 164,
        RMenu = 165,
        OEM_1 = 186,
        OEM_Plus = 187,
        OEM_Comma = 188,
        OEM_Minus = 189,
        OEM_Period = 190,
        OEM_2 = 191,
        OEM_3 = 192,
        OEM_4 = 219,
        OEM_5 = 220,
        OEM_6 = 221,
        OEM_7 = 222,
        OEM_8 = 223,
        OEM_102 = 226,
        Slash = 191,
        BackSlash = 220,
    };
}
namespace via::network::Codec {
    enum class Mode : int64_t {
        Dict = 0,
        Comp = 1,
    };
}
namespace via::path {
    enum class PathKind : int64_t {
        RelativeOrAbsolute = 0,
        Absolute = 1,
        Relative = 2,
    };
}
namespace app::ropeway::effect::script::LightningController {
    enum class STATE : int64_t {
        INVALID = 0,
        READY = 1,
        PLAY = 2,
        STOP = 3,
    };
}
namespace via::effect::script::EffectManager {
    enum class LODType : int64_t {
        ObjectEffectManager = 0,
        EnviromentEffectManager = 1,
    };
}
namespace app::ropeway::NaviMoveSupporter::PortalCorrect {
    enum class CorrectType : int64_t {
        Average = 0,
        WeightCoefAverage = 1,
        OnlyFarthest = 2,
        OnlyNearest = 3,
    };
}
namespace via::motion::DevelopRetargetResource {
    enum class RetargetJointName : int64_t {
        HeadName = 0,
        NeckName = 1,
        Spine0Name = 2,
        Spine1Name = 3,
        Spine2Name = 4,
        Spine3Name = 5,
        Spine4Name = 6,
        HipsName = 7,
        LHandName = 8,
        LForeArmName = 9,
        LArmName = 10,
        LShoulderName = 11,
        RHandName = 12,
        RForeArmName = 13,
        RArmName = 14,
        RShoulderName = 15,
        LFootName = 16,
        LLegName = 17,
        LUpLegName = 18,
        RFootName = 19,
        RLegName = 20,
        RUpLegName = 21,
        MAX = 22,
    };
}
namespace via {
    enum class GroupUIStyle : int64_t {
        Box = 0,
        NoBorder = 1,
        Expander = 2,
    };
}
namespace via::relib::render {
    enum class UpdateOrder : int64_t {
        BaseOrder = -2000,
        MeshMaterialController = -1999,
    };
}
namespace via {
    enum class AccountPickerResult : int64_t {
        Disabled = 0,
        UserChanged = 1,
        UserUnchanged = 2,
        DeviceDisconnected = 3,
        Failed = 4,
    };
}
namespace via::str {
    enum class ComparisonType : int64_t {
        Ordinal = 0,
        OrdinalIgnoreCase = 1,
    };
}
namespace app::ropeway::enemy::em7300::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_BACKSTEP = 2,
        ACT_REACTION = 3,
        ACT_WALL_UP = 4,
        ACT_TRAIN_UP = 5,
        ATK_BACK = 2000,
        ATK_DASH = 2001,
        ATK_LANE_L = 2002,
        ATK_LANE_R = 2003,
        ATK_HIGH_R = 2004,
        ATK_MIDL_TWO = 2005,
        ATK_RAGE = 2006,
        ATK_RAGE_2 = 2007,
        ATK_BOTH_HAND = 2008,
        ATK_BATTLE_START = 2009,
        DMG_STUN = 3000,
        DMG_WALL_FALL = 3001,
        DMG_TRAIN_FALL = 3002,
        DMG_DOWN_LOOP = 3003,
        DMG_DEADDOWN = 3004,
        DIE_NORMAL = 5000,
        DIE_RIGID = 5001,
        DIE_NORMAL_ROGUE = 5002,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace via::wwise {
    enum class GameObjectIdIndex : int64_t {
        WwiseDriver = 1,
        WwiseManager = 2,
        Listener_0 = 3,
        Listener_1 = 4,
        Listener_2 = 5,
        Listener_3 = 6,
        Listener_4 = 7,
        Listener_5 = 8,
        Listener_6 = 9,
        Listener_7 = 10,
        Offset = 11,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class Lounge : int64_t {
        LOUNGE_00 = 0,
        LOUNGE_01 = 1,
        LOUNGE_02 = 2,
        LOUNGE_03 = 3,
    };
}
namespace app::ropeway::gamemastering::SaveDataManager {
    enum class SaveMode : int64_t {
        SYSTEM = 0,
        SCENARIO = 1,
        FOURTH = 2,
        TOFU = 3,
        ROGUE = 4,
        TEMP = 5,
        DEBUG = 6,
        STGJMP1 = 7,
        STGJMP2 = 8,
        STGJMP3 = 9,
        STGJMP4 = 10,
        INVALID = 11,
    };
}
namespace app::Collision::CollisionSystem {
    enum class AsyncCastRayMode : int64_t {
        IsHit = 0,
        NearHitDetail = 1,
        AllHitsDetail = 2,
    };
}
namespace via::effect::detail {
    enum class EmitterExecuteState : int64_t {
        Update = 1,
        Reset = 2,
        Kill = 4,
        Stop = 8,
    };
}
namespace app::ropeway::gui::TitleFirstBehavior {
    enum class DecidedResult : int64_t {
        NEW_GAME_LEON = 0,
        NEW_GAME_CLAIRE = 1,
        COSTUME = 2,
        OPTIONS = 3,
        MAX = 4,
        CANCEL = 5,
    };
}
namespace via::navigation::AIMapEffector {
    enum class NeedUpdate : int64_t {
        No = 0,
        Yes = 1,
        Force = 2,
    };
}
namespace app::ropeway::gui::RogueGeneralAnnounceBehavior {
    enum class ObserveType : int64_t {
        INVALID = 0xFFFFFFFF,
        NONE = 0,
        BOSS_STAGE = 1,
        GAME_START = 2,
        GAME_FAILED = 3,
        GAME_CLEAR = 4,
    };
}
namespace app::ropeway::enemy::em4400::Em4400ThinkChild {
    enum class MotionDefine : int64_t {
        Idle = 0,
        Idle_1 = 1,
        Idle_2 = 2,
        Walk = 101,
        Turn = 102,
        Fly = 500,
        Hide = 510,
    };
}
namespace app::ropeway::enemy::em0000::RoleAction {
    enum class EatStandup : int64_t {
        STANDUP_PL = 0,
        STANDUP_F = 1,
        STANDUP_B = 2,
        STANDUP_L = 3,
        STANDUP_R = 4,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class TerritoryTurnBackPointType : int64_t {
        NONE = 0,
        SET_POSITION = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickDoorBase::DoorEvent {
    enum class BootType : int64_t {
        BY_SWITCH = 0,
        BY_ON = 1,
    };
}
namespace app::ropeway::CatalogManager {
    enum class Phase : int64_t {
        Initial = 0,
        FirstKick = 1,
        Working = 2,
    };
}
namespace app::ropeway::SceneStandbyController {
    enum class PrioritySetMode : int64_t {
        Method = 0,
        Value = 1,
    };
}
namespace app::ropeway::NoticePointController {
    enum class JointStateKind : int64_t {
        None = 0,
        Enable = 1,
        Disable = 2,
    };
}
namespace app::ropeway::fsm {
    enum class OwnerFsmComponentType : int64_t {
        Fsm = 1,
        MotionFsm = 2,
        MotionJackFsm = 4,
        Other = 1073741824,
    };
}
namespace app::ropeway::enemy::savedata::EmSaveData {
    enum class HitPointDataIndex : int64_t {
        DEFAULT_HITPOINT = 0,
        CURRENT_HITPOINT = 1,
        CURRENT_WINCEPOINT = 2,
        __ARRAY_SIZE__ = 3,
    };
}
namespace app::ropeway::enemy::em7000::Em7000Define {
    enum class CounterIndex : int64_t {
        CounterIndex_00 = 0,
        CounterIndex_01 = 1,
        CounterIndex_02 = 2,
        CounterIndex_03 = 3,
        CounterIndex_Max = 4,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class MaterialOption : int64_t {
        INVALID = 0xFFFFFFFF,
        BLOODY_FACE_GAMEOVER = 0,
    };
}
namespace app::ropeway::gui::FloorMapSt41Behavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_4_101_0a = 1,
        Room_4_101_0b = 2,
        Room_4_201_0a = 3,
        Room_4_201_0b = 4,
        Room_4_201_0c = 5,
        Room_4_201_0d = 6,
        Room_4_202_0 = 7,
        Room_4_204_0 = 8,
        Room_4_206_0a = 9,
        Room_4_206_0b = 10,
        Room_4_207_0 = 11,
        Room_4_208_0 = 12,
        Room_4_209_0 = 13,
        Room_4_210_0 = 14,
        Room_4_211_0 = 15,
        Room_4_212_0 = 16,
        Room_4_214_0 = 17,
        Room_4_215_0 = 18,
        Room_4_216_0 = 19,
        Room_4_217_0 = 20,
        Room_4_301_0 = 21,
        Room_4_303_0 = 22,
        Room_4_304_0 = 23,
        Room_4_305_0 = 24,
        Room_4_306_0 = 25,
        Room_4_307_0 = 26,
        Room_4_308_0 = 27,
        Room_4_309_0a = 28,
        Room_4_309_0b = 29,
        Room_4_311_0 = 30,
        Room_4_312_0 = 31,
        Room_4_313_0 = 32,
        Room_4_314_0 = 33,
    };
}
namespace via::nnfc::nfp {
    enum class NfpApplet : int64_t {
        Invalid = 0,
        Register = 1,
        Remove = 2,
        Restore = 3,
    };
}
namespace app::ropeway::weapon::shell::ShellBase {
    enum class BombEmberShellStatusInformation : int64_t {
        NoSuspend = 0,
        Update = 1,
        LateUpdate = 2,
    };
}
namespace app::ropeway::timeline::TimelineDefine {
    enum class EV : int64_t {
        EV000 = 20000,
        EV011 = 20011,
        EV020 = 20020,
        EV030 = 20030,
        EV040 = 20040,
        EV050 = 20050,
        EV051 = 20051,
        EV100 = 20100,
        EV110 = 20110,
        EV140 = 20140,
        EV220 = 20220,
        EV250 = 20250,
        EV251 = 20251,
        EV260 = 20260,
        EV330 = 20330,
        EV400 = 20400,
        EV410 = 20410,
        EV530 = 20530,
        EV580 = 20580,
        EV620 = 20620,
        EV680 = 20680,
        EV690 = 20690,
        EV700 = 20700,
        EV710 = 20710,
        EV730 = 20730,
        EV740 = 20740,
        EV750 = 20750,
        EV760 = 20760,
        EV890 = 20890,
        EV891 = 20891,
        EV900 = 20900,
        EV905 = 20905,
        EV980 = 20980,
        EV981 = 20981,
        EV982 = 20982,
        EV983 = 20983,
        EV984 = 20984,
        EV985 = 20985,
        EV986 = 20986,
        EV987 = 20987,
        EV990 = 20990,
        EV991 = 20991,
        EV992 = 20992,
        EV993 = 20993,
        Invalid = 0xFFFFFFFF,
    };
}
namespace app::ropeway::enemy::tracks::Em9000ColliderTrack {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Press = 0,
        Catch = 1,
        Sensor_Touch = 2,
        Sensor_Gimmick = 3,
        DrawControl = 4,
        BackFind = 5,
        CatchLarge = 6,
        BackFindLarge = 7,
        CatchMiddle = 8,
        CatchLong = 9,
    };
}
namespace via::network::wrangler {
    enum class PopulationSample : int64_t {
        UseProviderPopulationSample = -2,
        UseSystemPopulationSample = 0xFFFFFFFF,
        Undefined = 0,
    };
}
namespace app::ropeway::InputDefine {
    enum class Trigger : int64_t {
        INVALID = 0,
        LEFT = 1,
        RIGHT = 2,
    };
}
namespace app::ropeway::gimmick::option::WeaponMotionControlSettings::Param {
    enum class OrderKind : int64_t {
        None = 0,
        Load = 1,
        Unload = 2,
    };
}
namespace app::ropeway::gui::GameOverBehavior {
    enum class DecidedResult : int64_t {
        TO_TITLE_0 = 0,
        LOAD = 1,
        CONTINUE_0 = 2,
        TO_TITLE_1 = 3,
        CONTINUE_1 = 4,
        TO_TITLE_2 = 5,
        CONTINUE_2 = 6,
        MAX = 7,
    };
}
namespace app::ropeway::gui::RogueInventorySlotBehavior {
    enum class SlotSelectSubMode : int64_t {
        Default = 0,
        RuckSack = 1,
        InGameShop = 2,
    };
}
namespace via::navigation::NavigationSurface {
    enum class PortalOffsetSizeType : int64_t {
        Ratio = 0,
        Distance = 1,
    };
}
namespace via::render::RichDecal {
    enum class HeightBlendMode : int64_t {
        Ignore = 0,
        Add = 1,
        Mul = 2,
    };
}
namespace app::ropeway::fsmv2::TriggerSoundForLevel {
    enum class PositionType : int64_t {
        Position = 0,
        Follow = 1,
    };
}
namespace app::ropeway::gimmick::option::EnemyConditionCheckSettings::EnemyConditionCheckParam {
    enum class DistanceCheckType : int64_t {
        RangeIN_ = 0,
        RangeOver = 1,
    };
}
namespace app::ropeway::enemy::tracks::Em7000ColliderTrack {
    enum class ColliderGroup_G : int64_t {
        Damage = 0,
        AimTarget = 1,
    };
}
namespace app::ropeway::gamemastering::TutorialManager {
    enum class DrawPosition : int64_t {
        DOWN = 0,
        LEFT = 1,
        RIGHT = 2,
        UP = 3,
        MAX = 4,
    };
}
namespace via::AABB {
    enum class EdgeId : int64_t {
        EdgeId_011_TO_111 = 0,
        EdgeId_111_TO_101 = 1,
        EdgeId_101_TO_001 = 2,
        EdgeId_001_TO_011 = 3,
        EdgeId_010_TO_110 = 4,
        EdgeId_110_TO_100 = 5,
        EdgeId_100_TO_000 = 6,
        EdgeId_000_TO_010 = 7,
        EdgeId_010_TO_011 = 8,
        EdgeId_110_TO_111 = 9,
        EdgeId_100_TO_101 = 10,
        EdgeId_000_TO_001 = 11,
        EdgeId_Max = 12,
    };
}
namespace app::ropeway::gimmick::action::GimmickBreakable {
    enum class AttackParam : int64_t {
        TO_EM_EXPLOSION = 0,
        TO_PL_EXPLOSION = 1,
        TO_EM_BLAST = 2,
        TO_PL_BLAST = 3,
        MAX = 4,
    };
}
namespace via::render {
    enum class DefaultResolution : int64_t {
        DefaultResolution_1080p = 1080,
        DefaultResolution_1260p = 1260,
        DefaultResolution_1440p = 1440,
        DefaultResolution_1620p = 1620,
        DefaultResolution_1800p = 1800,
        DefaultResolution_1890p = 1890,
        DefaultResolution_1980p = 1980,
        DefaultResolution_2070p = 2070,
        DefaultResolution_2160p = 2160,
    };
}
namespace app::ropeway::EnemyRelocationManager {
    enum class SetGroundType : int64_t {
        Ground = 0,
        Wall = 1,
        Ceiling = 2,
    };
}
namespace app::ropeway::EquipmentDefine {
    enum class Bullet : int64_t {
        None = 0,
        Handgun = 1,
        Shotgun = 2,
        SubMachinegun = 4,
        Magnum = 8,
        AcidGrenade = 16,
        FireGrenade = 32,
        EnergyPack = 64,
        Battery = 128,
        Fuel = 256,
        Handgun2 = 512,
        Magnum2 = 1024,
        Rocket = 2048,
        Minigun = 4096,
    };
}
namespace app::ropeway::enemy::em0000::RoleAction {
    enum class WindowSealed : int64_t {
        REACH = 0,
        NONE = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickBreakState {
    enum class StateType : int64_t {
        BEFORE = 0,
        BREAKING = 1,
        AFTER = 2,
    };
}
namespace via::hid::detail::SceneObserver {
    enum class SceneStatus : int64_t {
        Enabled = 0,
        Disabled = 1,
        Paused = 2,
        Resumed = 3,
        Construct = 4,
        Preview = 5,
    };
}
namespace app::ropeway::gui::RogueBaseAreaBehavior {
    enum class BaseAreaListItem : int64_t {
        SHOP = 0,
        STORAGE = 1,
        FORMATION = 2,
        GAMESTART = 3,
    };
}
namespace via::hid {
    enum class NpadJoyAssignmentMode : int64_t {
        Dual = 0,
        Single = 1,
        None = 2,
    };
}
namespace via::behaviortree::SelectorSequence {
    enum class RunType : int64_t {
        Loop = 0,
        Once = 1,
        OnceEnd = 2,
    };
}
namespace app::ropeway::enemy::em4400::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_RUN = 1,
        DIE_NORMAL = 5000,
        DIE_RIGID = 5001,
    };
}
namespace via::render {
    enum class IndexBufferFormat : int64_t {
        U16 = 0,
        U32 = 1,
        Unknown = 2,
    };
}
namespace app::ropeway::gimmick::option::GimmickValueCheckSettings::Param {
    enum class ValueType : int64_t {
        Int = 0,
        UInt = 1,
        Float = 2,
        Boolean = 3,
        String = 4,
    };
}
namespace via::CoreEntry {
    enum class DevelopPlatform : int64_t {
        Default = 0,
        X64 = 1,
        PS4 = 2,
        XB1 = 3,
        UWP = 4,
        NSW = 5,
        WGM = 6,
    };
}
namespace app::ropeway::gui::NewInventoryBehavior {
    enum class GimmickMode : int64_t {
        None = 0,
        Use = 1,
        UseDispPlayer = 2,
    };
}
namespace app::ropeway::weapon::shell::ShellDefine {
    enum class ShellCategory : int64_t {
        Ballistic = 0,
        Spread = 1,
    };
}
namespace app::ropeway::enemy::em7300::tracks::Em7300ScrollMoveTrack {
    enum class ScrollType : int64_t {
        NONE = 0,
        DOWN_SCROLL = 1,
    };
}
namespace app::ropeway::CollisionSeDefine {
    enum class HIT_TYPE : int64_t {
        CUT = 0,
        BLOW = 1,
        OTHER = 2,
        NONE = 3,
    };
}
namespace via::effect::gpgpu::detail {
    enum class ShapeType : int64_t {
        AABB = 0,
        OBB = 1,
        Sphere = 2,
    };
}
namespace via::effect::lensflare {
    enum class FilterType : int64_t {
        Global = 0,
        Glow = 1,
        Iris = 2,
        MultiIris = 3,
        Hoop = 4,
        Max = 5,
    };
}
namespace app::ropeway::gui::FloorMapSt41Behavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        sm40_111_RPDFrontDoor01A = 1,
        Door_2_1_001 = 2,
        Door_2_1_003 = 3,
        Door_2_1_004 = 4,
        Door_2_1_005 = 5,
        Door_2_1_006w = 6,
        Door_2_1_007 = 7,
        Door_2_1_008 = 8,
        Door_2_1_009 = 9,
        Door_2_1_010 = 10,
        Door_2_1_011 = 11,
        Door_2_1_012w = 12,
        Door_2_1_013 = 13,
        Door_2_1_015w = 14,
        Door_2_1_018 = 15,
        Door_2_1_019 = 16,
        Door_2_1_020 = 17,
        Door_2_1_022 = 18,
        Door_2_1_025 = 19,
        Door_2_1_028 = 20,
        Door_2_1_030w = 21,
        Door_2_1_032 = 22,
        Door_2_1_034 = 23,
        Door_2_1_040 = 24,
    };
}
namespace via::hid::camera {
    enum class PlayStationCameraConfigType : int64_t {
        Unknown = 0,
        Type1 = 1,
        Type2 = 2,
        Type3 = 3,
        Type4 = 4,
        Extention = 16,
    };
}
namespace app::ropeway::enemy::em5000 {
    enum class WeakPartsBreakReaction : int64_t {
        Totter = 0,
        Nockback = 1,
        Nockback_Violently = 2,
        Nockback_Target = 3,
        Max = 4,
    };
}
namespace app::ropeway::gui::ItemPortableSafeBehavior {
    enum class JointEnum : int64_t {
        button_p008 = 0,
        button_p007 = 1,
        button_p006 = 2,
        button_p005 = 3,
        button_p004 = 4,
        button_p003 = 5,
        button_p002 = 6,
        button_p001 = 7,
        Max = 8,
    };
}
namespace via::physics::Shape {
    enum class State : int64_t {
        Enabled = 0,
        Max = 1,
    };
}
namespace via::os::http_client {
    enum class Error : int64_t {
        Unavailable = -2,
        Pending = 0xFFFFFFFF,
        None = 0,
    };
}
namespace app::ropeway::RogueEnemySpawnController {
    enum class ActivatePattern : int64_t {
        Default = 0,
        TrainingOnly = 1,
        NormalOnly = 2,
    };
}
namespace app::ropeway::gui::OptionPresetBehavior {
    enum class Result : int64_t {
        RECOMMENDATION = 0,
        HIGHEST = 1,
        HIGH = 2,
        MIDDLE = 3,
        LOW = 4,
        MAX = 5,
        CANCELED = 6,
    };
}
namespace via::motion::IkSpine::ManageJoint {
    enum class JOINT_TYPE : int64_t {
        NON = 0,
        START = 1,
        RAY = 2,
        END = 3,
    };
}
namespace app::ropeway::fsmv2::MotionFsmAction_SetChainEnable {
    enum class SetTiming : int64_t {
        START = 0,
        END = 1,
    };
}
namespace via::gui::detail {
    enum class LetterAlignmentV : int64_t {
        Top = 0,
        Center = 1,
        Bottom = 2,
        Baseline = 3,
    };
}
namespace via::navigation::NavigationWaypoint {
    enum class SearchRegion : int64_t {
        None = 0,
        Only = 1,
        Priority = 2,
    };
}
namespace app::ropeway::timeline::TimelinePreparation::PresetInfo {
    enum class State : int64_t {
        Init = 0,
        Standby = 1,
        Preset = 2,
        InActing = 3,
        Finish = 4,
    };
}
namespace app::ropeway::enemy::em6000::RoleAction {
    enum class Pattern : int64_t {
        IDLE = 0,
        FLOATATION = 1,
        CAVE_APPEAR = 2,
        CAVE_APPEAR2 = 3,
        CEILING_APPEAR = 4,
        FIRST_DIVE = 5,
        SWIM_APPEAR = 6,
        DRINK_SEWAGE = 7,
    };
}
namespace app::ropeway::enemy::em6000::fsm::tag {
    enum class StateAttr : int64_t {
        Swim = 1614742265,
        Vomit = 1594678060,
        WalkContinue = 614595396,
        NoGunSensor = 2811006706,
    };
}
namespace app::ropeway::gui::FloorMapSt5CBehavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_5_201_0 = 1,
        Room_5_202_0 = 2,
        Room_5_203_0a = 3,
        Room_5_203_0b = 4,
        Room_5_204_0a = 5,
        Room_5_204_0b = 6,
        Room_5_211_0a = 7,
        Room_5_211_0b = 8,
        Room_5_211_0c = 9,
        Room_5_212_0 = 10,
    };
}
namespace app::ropeway::enemy::em6300::Em6300Think {
    enum class FLAG : int64_t {
        AttackHited = 0,
        Threat = 1,
        TheEndMode = 2,
        FirstRocketLauncherDamage = 3,
    };
}
namespace app::ropeway::gamemastering::StaffRollToResetFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        WAIT_GUI_CREATION = 1,
        UPDATE = 2,
        FINALIZE = 3,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace app::ropeway::fsmv2 {
    enum class OwnerFsmComponentType : int64_t {
        Fsm = 1,
        MotionFsm = 2,
        MotionJackFsm = 4,
        Other = 1073741824,
    };
}
namespace app::ropeway::enemy::userdata::Em6200AttackUserData {
    enum class AttackPatternType : int64_t {
        Hold = 0,
        Throw_R = 1,
        Throw_L = 2,
        Down_Crush = 3,
    };
}
namespace app::ropeway::gimmick::option::OuterDynamicMotionBankSettings::Info {
    enum class TargetKind : int64_t {
        Player = 0,
        Partner = 1,
        Invalid = 2,
    };
}
namespace via::motion {
    enum class JointExType : int64_t {
        None = 0,
        Rotation = 1,
        RotToScale = 2,
        RotToScaleEx = 3,
        RotToTrans = 4,
        RotToTransEx = 5,
        Finger = 6,
        Thumb = 7,
        RotToRot = 8,
        RotToRotEx = 9,
        Limit = 10,
        PointConstraint = 11,
        ParentConstraint = 12,
        BsplineConstraint = 13,
        RemapValue = 14,
        MultiRemapValue = 15,
        Rotation2 = 16,
    };
}
namespace app::ropeway::Em6100Think {
    enum class FLAG : int64_t {
        AttackHited = 0,
        DamageHited = 1,
        DivingRequest = 2,
        OnGround = 3,
        InWater = 4,
    };
}
namespace System::Runtime::InteropServices {
    enum class LayoutKind : int64_t {
        Sequential = 0,
        Explicit = 2,
        Auto = 3,
    };
}
namespace app::ropeway::Em4100Define {
    enum class RequestSetID : int64_t {
        PRESS = 0,
        DAMAGE = 1,
        SENSOR_TOUCH = 2,
        SENSOR_HEARING = 3,
        ATTACK_BITE = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickDialLockBehavior {
    enum class State : int64_t {
        Init = 0,
        Sleep = 1,
        Wait = 2,
        MoveUp = 3,
        MoveDown = 4,
        ResultSuccess = 5,
        ResultFailure = 6,
        Dead = 7,
    };
}
namespace app::ropeway::gui::ScreenSettingBehavior {
    enum class EndType : int64_t {
        DECIDED = 0,
        CANCELED = 1,
        ILLEGAL = 2,
    };
}
namespace via::gui::detail {
    enum class MaskLayer : int64_t {
        Default = 0,
        Top = 1,
        TopMost = 2,
        Max = 3,
    };
}
namespace System::Diagnostics::DebuggableAttribute {
    enum class DebuggingModes : int64_t {
        None = 0,
        Default = 1,
        DisableOptimizations = 2,
        IgnoreSymbolStoreSequencePoints = 3,
        EnableEditAndContinue = 4,
    };
}
namespace via::motion {
    enum class MotionAppendDataClassType : int64_t {
        Unknown = 0,
        Vec2 = 1,
        Vec3 = 2,
        Vec4 = 3,
        Quaternion = 4,
        Matrix = 5,
    };
}
namespace app::ropeway::SweetLightManager {
    enum class Channel : int64_t {
        Player = 0,
        Environment = 1,
        Max = 2,
    };
}
namespace via::motion::tree {
    enum class NodeType : int64_t {
        Unknown = 0,
        Motion = 1,
        Param = 2,
    };
}
namespace app::ropeway::weapon::shell::BallisticSettingBase {
    enum class HitTerrain : int64_t {
        VOLUME = 0,
        POSITION = 1,
    };
}
namespace app::ropeway::camera::CameraDefine {
    enum class BankID : int64_t {
        CUT_SCENE = 0,
        GIMMICK = 10,
        ACTION = 20,
    };
}
namespace via::areamap::test::DistanceFromQueryRegion {
    enum class Condition : int64_t {
        Mode_Closest = 0,
        Mode_Farthest = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickSwitch {
    enum class MaterialType : int64_t {
        GREEN = 0,
        RED = 1,
        MAX = 2,
    };
}
namespace app::ropeway {
    enum class IkWristKind : int64_t {
        ARMFIT = 0,
        FOUR_LEG = 1,
        FOUR_LEG_WITH_ATTITUDE = 2,
    };
}
namespace System::Security::Permissions {
    enum class SecurityAction : int64_t {
        Demand = 2,
        Assert = 3,
        Deny = 4,
        PermitOnly = 5,
        LinkDemand = 6,
        InheritanceDemand = 7,
        RequestMinimum = 8,
        RequestOptional = 9,
        RequestRefuse = 10,
    };
}
namespace app::ropeway::EnvironmentStandbyController {
    enum class AreaLoadReason : int64_t {
        None = 0,
        Current = 1,
        CurrentAdditional = 2,
        Next = 3,
        NextAdditional = 4,
        Standby = 5,
        LoadMapZone = 6,
        LoadMapZoneFromTheHall = 7,
        ChangeMapSpace = 8,
        DirectRequest = 9,
    };
}
namespace app::ropeway::weapon::shell::ShellDefine::Group {
    enum class RankLayer : int64_t {
        Top = 0,
        Upper_1 = 1000,
        Upper_2 = 2000,
        Upper_3 = 3000,
        Upper_4 = 4000,
        Upper_5 = 5000,
        Upper_6 = 6000,
        Upper_7 = 7000,
        Upper_8 = 8000,
        Upper_9 = 9000,
        Default = 10000,
        Lower_1 = 11000,
        Lower_2 = 12000,
        Lower_3 = 13000,
        Lower_4 = 14000,
        Lower_5 = 15000,
        Lower_6 = 16000,
        Lower_7 = 17000,
        Lower_8 = 18000,
        Lower_9 = 19000,
        Bottom = 20000,
    };
}
namespace app::ropeway::gimmick::action::GimmickDoorOptions {
    enum class SideType : int64_t {
        SIDE_A = 0,
        SIDE_B = 1,
        BOTH_SIDE = 2,
    };
}
namespace app::ropeway::enemy::em0000::fsmv2::action::Em0000MFsmAction_ActionCameraPlay {
    enum class CameraMode : int64_t {
        MOTION_ID = 0,
        VARIABLE_MOTION_ID = 1,
    };
}
namespace via::eq::Var {
    enum class Type : int64_t {
        Float = 0,
        Color = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickStatueOfWisdomControl {
    enum class Rno : int64_t {
        INIT = 0,
        WAIT_USE = 1,
        WAIT_OPEN = 2,
        OPEN = 3,
        FINISHED = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickAction::PrefabInstantiateWork {
    enum class Routine : int64_t {
        INIT = 0,
        REQUEST = 1,
        WAIT = 2,
        CREATED = 3,
        ERR_NO_DATA = 4,
        ERR_NO_OWNER = 5,
    };
}
namespace app::ropeway::enemy::em6000 {
    enum class RankParamHash : int64_t {
        MOVE_SPEED_SCALE = 225628402,
        WINCE_RATE = 2531057400,
        EYE_EXPOSURE_SET = 2919377961,
        CHILDREN_VOMIT_RATE = 3609457783,
        CHILDREN_POISON_RATE = 557429514,
    };
}
namespace app::ropeway::gimmick::action::GimmickDialLockBehavior {
    enum class CylinderChars : int64_t {
        ABCDEF = 0,
        FGHIJK = 1,
        KLMNOP = 2,
        PQRSTU = 3,
        UVWXYZ = 4,
        MAX = 5,
    };
}
namespace via {
    enum class Language : int64_t {
        Japanese = 0,
        English = 1,
        French = 2,
        Italian = 3,
        German = 4,
        Spanish = 5,
        Russian = 6,
        Polish = 7,
        Dutch = 8,
        Portuguese = 9,
        PortugueseBr = 10,
        Korean = 11,
        TransitionalChinese = 12,
        SimplelifiedChinese = 13,
        Finnish = 14,
        Swedish = 15,
        Danish = 16,
        Norwegian = 17,
        Czech = 18,
        Hungarian = 19,
        Slovak = 20,
        Arabic = 21,
        Turkish = 22,
        Bulgarian = 23,
        Greek = 24,
        Romanian = 25,
        Thai = 26,
        Ukrainian = 27,
        Max = 28,
        Unknown = 28,
    };
}
namespace app::ropeway::enemy::em3000::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_CAUTION = 1,
        ACT_SEARCHING = 2,
        ACT_THREAT = 3,
        ACT_MOVE = 4,
        ACT_BACKWALK = 5,
        ACT_STEP = 6,
        ACT_TRUN = 7,
        ACT_TRUN180 = 8,
        ACT_JUMP_GROUND_TO_CEILING = 9,
        ACT_JUMP_GROUND_TO_WALL = 10,
        ACT_PERSIST = 11,
        ATK_CLAW = 2000,
        ATK_JUMP_CLAW = 2001,
        ATK_JUMP_STICK_CLAW = 2002,
        ATK_TONGUE = 2003,
        ATK_JUMP_AHEAD = 2004,
        ATK_HOLD = 2005,
        ATK_HOLD_BEHEAD = 2006,
        ATK_JUMP_DOOR = 2007,
        SPT_KNIFE = 4000,
        SPT_GRENADE = 4001,
        DMG_S = 3000,
        DMG_KNOCKBACK = 3001,
        DMG_BURST = 3002,
        DMG_STANDUP = 3003,
        DMG_FALL_CELLING = 3004,
        DMG_FALL_WALL = 3005,
        DMG_FLASH = 3006,
        DMG_BURN = 3007,
        DMG_ACID = 3008,
        DMG_SHOCK = 3009,
        DMG_SHOCK_DOWN = 3010,
        DIE_NORMAL = 5000,
        DIE_RIGID = 5001,
        SET_IDLE = 6000,
        SET_FIRST_APPEARANCE = 6001,
        SET_FALL = 6002,
        SET_DOG_EAT = 6003,
        SET_HOLD_CEILING = 6004,
        SET_FALL_TOPBOARD = 6005,
        SET_INTRUSION_TOPBOARD = 6006,
        SET_DEAD = 6007,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace via::motion::IkLookAt {
    enum class ViewState : int64_t {
        None = 0,
        Move = 1,
        Follow = 2,
    };
}
namespace via::clr {
    enum class MethodSemanticsFlag : int64_t {
        Setter = 1,
        Getter = 2,
        Other = 4,
        AddOn = 8,
        RemoveOn = 16,
        Fire = 32,
    };
}
namespace app::ropeway::enemy::em7300::Em7300Wall {
    enum class GimmickType : int64_t {
        WALL = 0,
        WALL_TOP = 1,
        WALL_UP = 2,
        TRAIN_UP = 3,
        CENTER_OF_STAGE = 4,
    };
}
namespace via::curve {
    enum class EaseType : int64_t {
        Linear = 0,
        InSine = 1,
        OutSine = 2,
        InOutSine = 3,
        InQuad = 4,
        OutQuad = 5,
        InOutQuad = 6,
        InCubic = 7,
        OutCubic = 8,
        InOutCubic = 9,
        InQuart = 10,
        OutQuart = 11,
        InOutQuart = 12,
        InQuint = 13,
        OutQuint = 14,
        InOutQuint = 15,
        InExpo = 16,
        OutExpo = 17,
        InOutExpo = 18,
        InCirc = 19,
        OutCirc = 20,
        InOutCirc = 21,
        InBack = 22,
        OutBack = 23,
        InOutBack = 24,
        InElastic = 25,
        OutElastic = 26,
        InOutElastic = 27,
        InBounce = 28,
        OutBounce = 29,
        InOutBounce = 30,
    };
}
namespace app::ropeway::Log {
    enum class Level : int64_t {
        Critical = 2,
        ERROR_ = 3,
        Warning = 4,
        Notice = 5,
        Info = 6,
        Debug = 7,
        WarningOnlyLog = 8,
    };
}
namespace app::ropeway::gimmick::action::GimmickAutoDoor {
    enum class MoveType : int64_t {
        JOINT_CONTROL = 0,
        MOTION = 1,
    };
}
namespace app::ropeway::gui::FloatIconBehavior {
    enum class ButtonType : int64_t {
        Invalid = 0,
        OBJECT_ICON = 1,
        RL = 2,
        RL_HOLD = 3,
        RL_LONG = 4,
        RL_DISABLE = 5,
        RD = 6,
        RD_HOLD = 7,
        RD_LONG = 8,
        RD_DISABLE = 9,
        RR = 10,
        RR_HOLD = 11,
        RR_LONG = 12,
        RR_DISABLE = 13,
        RU = 14,
        RU_HOLD = 15,
        RU_LONG = 16,
        RU_DISABLE = 17,
        R1 = 18,
        R1_HOLD = 19,
        R1_LONG = 20,
        R1_DISABLE = 21,
        R2 = 22,
        R2_HOLD = 23,
        R2_LONG = 24,
        R2_DISABLE = 25,
    };
}
namespace app::ropeway::gui::CombatIconBehavior {
    enum class State : int64_t {
        NONE = 0,
        TO_ON = 1,
        DRAWING = 2,
        TO_OFF = 3,
    };
}
namespace via::dialog::core {
    enum class dummy : int64_t {
    };
}
namespace via::render::VolumeDecal {
    enum class OpacityFlag : int64_t {
        None = 0,
        BaseColor = 1,
        Normal = 2,
        Emissive = 8,
        BaseColorAndNormal = 3,
        BaseColorAndEmissive = 9,
        NormalAndEmissive = 10,
        All = 11,
    };
}
namespace app::ropeway::network::service::RichpresenceServiceController {
    enum class FlowType : int64_t {
        Unknown = 0,
        InGame = 1,
        Menu = 2,
    };
}
namespace via::hid::HIDEntry {
    enum class RemotePlayKeyAssign : int64_t {
        PatternA = 0,
        PatternB = 1,
        PatternC = 2,
        PatternD = 3,
        PatternE = 4,
        PatternF = 5,
        PatternG = 6,
        PatternH = 7,
    };
}
namespace app::ropeway::enemy::EnemyWeaponDataUserData::WeaponValueInfoBase {
    enum class GrenadeLauncherShellType : int64_t {
        Bullet = 0,
        Acid = 1,
        Fire = 2,
    };
}
namespace via::relib::effect::behavior::ReLibMarkStampController {
    enum class SaveStateEnum : int64_t {
        None = 0,
        Preparing = 1,
        Ready = 2,
    };
}
namespace via::gui {
    enum class GridInputDirection : int64_t {
        Up = 0,
        Right = 1,
        Down = 2,
        Left = 3,
    };
}
namespace via {
    enum class SystemServiceOperationMode : int64_t {
        Handheld = 0,
        Console = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickSwitch {
    enum class Routine : int64_t {
        INIT = 0,
        TURN_ON = 1,
        TURN_OFF = 2,
        SLEEP = 3,
        TRY = 4,
        MAX = 5,
    };
}
namespace via::effect::gpgpu::detail {
    enum class VectorFieldFlags : int64_t {
        Tiled = 1,
        Local = 2,
    };
}
namespace app::ropeway::fsmv2::condition::FigureMotionChangeCondition {
    enum class MotionChangeInputType : int64_t {
        NONE = 0,
        R2 = 1,
        L2 = 2,
        R2orL2 = 3,
        DECIDE = 4,
        CANCEL = 5,
        AUTO = 6,
    };
}
namespace app::ropeway::enemy::em6200::Em6200ChaserController {
    enum class STATE : int64_t {
        Wait = 0,
        Move = 1,
        DoorOpening = 2,
        DoorPassing = 3,
        GhostDoorOpening = 4,
        Stop = 5,
    };
}
namespace app::Collision::CollisionSystem::AsyncCastRequestBase {
    enum class CastType : int64_t {
        Invalid = 0,
        Ray = 1,
        Sphere = 2,
    };
}
namespace via::areamap::test::DistanceFromItemWithTag {
    enum class Limit : int64_t {
        CurrentArea = 0,
        AllAreas = 1,
    };
}
namespace via::keyframe::ChannelBase {
    enum class DataType : int64_t {
        Bool = 0,
        S8 = 1,
        U8 = 2,
        S16 = 3,
        U16 = 4,
        S32 = 5,
        U32 = 6,
        S64 = 7,
        U64 = 8,
        F32 = 9,
        F64 = 10,
        Str8 = 11,
        Str16 = 12,
        Vec2 = 13,
        Vec3 = 14,
        Vec4 = 15,
        Quaternion = 16,
    };
}
namespace app::ropeway::gimmick::action::GimmickElevatorPartsBase {
    enum class State : int64_t {
        CLOSED = 0,
        OPENED = 1,
        CLOSE = 2,
        OPEN = 3,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0000_MONTAGE_PARTS_SHIRT : int64_t {
        NONE = 0,
        SHIRT00 = 1,
        SHIRT00_00_00 = 2,
        SHIRT01 = 3,
        SHIRT01_00_00 = 4,
        SHIRT01_01_00 = 5,
        SHIRT01_02_00 = 6,
        SHIRT01_03_00 = 7,
        SHIRT01_04_00 = 8,
        SHIRT02 = 9,
        SHIRT03 = 10,
        SHIRT03_00_00 = 11,
        SHIRT03_00_01 = 12,
        SHIRT03_01_00 = 13,
        SHIRT03_01_01 = 14,
        SHIRT03_02_00 = 15,
        SHIRT03_02_01 = 16,
        SHIRT03_03_00 = 17,
        SHIRT03_03_01 = 18,
        SHIRT03_04_00 = 19,
        SHIRT03_04_01 = 20,
        SHIRT03_05_00 = 21,
        SHIRT03_05_01 = 22,
        SHIRT04 = 23,
        SHIRT05 = 24,
        SHIRT06 = 25,
        SHIRT07 = 26,
        SHIRT08 = 27,
        SHIRT08_00_00 = 28,
        SHIRT08_00_01 = 29,
        SHIRT08_00_02 = 30,
        SHIRT08_00_03 = 31,
        SHIRT09 = 32,
        SHIRT10 = 33,
        SHIRT10_00_00 = 34,
        SHIRT11 = 35,
        SHIRT11_00_00 = 36,
        SHIRT11_01_00 = 37,
        SHIRT11_02_00 = 38,
        SHIRT11_03_00 = 39,
        SHIRT11_04_00 = 40,
        SHIRT12 = 41,
        SHIRT12_00_00 = 42,
        SHIRT12_01_00 = 43,
        SHIRT12_02_00 = 44,
        SHIRT12_03_00 = 45,
        SHIRT12_04_00 = 46,
        SHIRT13 = 47,
        SHIRT70 = 48,
        SHIRT70_00_00 = 49,
        SHIRT70_01_00 = 50,
        SHIRT70_02_00 = 51,
        SHIRT70_03_00 = 52,
        SHIRT70_04_00 = 53,
        SHIRT70_05_00 = 54,
        SHIRT70_06_00 = 55,
        SHIRT70_07_00 = 56,
        SHIRT70_08_00 = 57,
        SHIRT70_09_00 = 58,
        SHIRT71 = 59,
        SHIRT72 = 60,
        SHIRT73 = 61,
        SHIRT74 = 62,
        SHIRT75 = 63,
        SHIRT76 = 64,
    };
}
namespace app::ropeway::EnemyManager {
    enum class WwiseAttentionEnemyDistanceIndex : int64_t {
        ALL = 0,
        IN_CAMERA = 1,
        EM0000 = 2,
        EM3000 = 3,
        EM4000 = 4,
        EM5000 = 5,
        EM6000 = 6,
        EM6200 = 7,
        EM6300 = 8,
        EM7000 = 9,
        EM7100 = 10,
        EM7200 = 11,
        EM7300 = 12,
        EM7400 = 13,
        EM9000 = 14,
        NUM = 15,
    };
}
namespace via::hid::detail {
    enum class HidAllocatorGpuBuss : int64_t {
        Onion = 0,
        Garlic = 1,
    };
}
namespace app::ropeway::enemy::em0000::Em0000ShirtChainController {
    enum class ChainStatus : int64_t {
        DEFAULT = 0,
        FACE_DOWN = 1,
    };
}
namespace via::dialog {
    enum class ButtonType : int64_t {
        Ok = 0,
        YesNo = 1,
        None = 2,
        OkCancel = 3,
        Wait = 4,
        WaitCancel = 5,
    };
}
namespace via {
    enum class AppletResult : int64_t {
        None = 0,
        Done = 1,
        Skipped = 2,
        Canceled = 3,
        Failed = 4,
    };
}
namespace via::hid::camera {
    enum class PlayStationCameraStopResult : int64_t {
        OK = 1,
        Stoped = 1,
        ErrInternal = 2147487744,
    };
}
namespace app::ropeway::camera {
    enum class CameraOffsetType : int64_t {
        Camera = 0,
        CameraY = 1,
        Joint = 2,
        Object = 3,
    };
}
namespace via::motion::CppSampleTracks {
    enum class Test : int64_t {
        A = 0,
        B = 1,
        C = 2,
    };
}
namespace via::gui {
    enum class TextAnimationCondition : int64_t {
        Start = 0,
        Play = 1,
        Pause = 2,
        End = 3,
    };
}
namespace app::ropeway::emXXXXParam {
    enum class MOTION_LIST : int64_t {
        FaceDown_00 = 0,
        FaceDown_01 = 1,
        FaceDown_02 = 2,
        FaceUp_00 = 100,
        FaceUp_01 = 101,
        FaceUp_02 = 102,
        FaceUp_03 = 103,
        WallDown_00 = 200,
        WallDown_01 = 201,
        WallUp_00 = 300,
        WallUp_01 = 301,
        WallUp_02 = 302,
        Desk_00 = 400,
        Desk_01 = 401,
        Police_00_Start = 1000,
        Police_00_Move = 1001,
        Police_00_End = 1002,
        Eat_Face_Down_00 = 4501,
        Eat_Face_Down_01 = 4503,
        Eat_Face_Down_End = 4300,
        Eat_Face_Up_00 = 4511,
        Eat_Face_Up_End = 4350,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class SimplePoseType : int64_t {
        STANDING = 0,
        LIMPING = 1,
        KNEELING = 2,
        TRAILING = 3,
        CREEPING = 4,
    };
}
namespace app::ropeway::Em4100Define {
    enum class ActionStatus : int64_t {
        ACT_IDLE = 0,
        ACT_IDLE_1 = 1,
        ACT_REACTION_0 = 2,
        ACT_REACTION_1 = 3,
        ACT_PIVOT_TURN_L90 = 4,
        ACT_PIVOT_TURN_R90 = 5,
        ACT_PIVOT_TURN_L180 = 6,
        ACT_PIVOT_TURN_R180 = 7,
        ACT_FLY = 8,
        ACT_RUN_FLY = 9,
        ACT_GLIDE = 10,
        ACT_FLAP = 11,
        ACT_RISE = 12,
        ACT_LAND = 13,
        ACT_MOVE = 14,
        ACT_STANDUP_FROM_DOWN = 15,
        ACT_CITY_FLY = 16,
        ATK_ATTACK = 2000,
        DMG_DAMAGE = 3000,
        DIE_NORMAL = 5000,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace via::clr {
    enum class MethodImplFlag : int64_t {
        CodeTypeMask = 3,
        IL = 0,
        Native = 1,
        OPTIL = 2,
        Runtime = 3,
        ManagedMask = 4,
        Unmanaged = 4,
        Managed = 0,
        ForwardRef = 16,
        PreserveSig = 128,
        InternalCall = 4096,
        Synchronized = 32,
        NoInlining = 8,
        AggressiveInlining = 256,
        NoOptimization = 64,
        HasRetVal = 512,
        ExposeMember = 1024,
        EmptyCtor = 2048,
        ContainsGenericParameters = 8192,
        HasThis = 16384,
        Break = 32768,
    };
}
namespace app::ropeway::CollisionDefine {
    enum class AttackEnchantAttribute : int64_t {
        Poison = 1,
        Acid = 2,
        Burn = 4,
        Electric = 8,
        PairLinkRay = 16,
    };
}
namespace app::ropeway::posteffect::cascade::TimeOverLayer {
    enum class UpdateState : int64_t {
        Start = 0,
        Keep = 1,
        End = 2,
    };
}
namespace via::fsm {
    enum class FsmSelectorType : int64_t {
        Graph = 0,
    };
}
namespace app::ropeway::gamemastering::Floor {
    enum class ID : int64_t {
        invalid = 0,
        RPD_A = 1,
        RPD_B = 2,
        RPD_C = 3,
        RPD_D = 4,
        RPD_E = 5,
        RPD_A_SECRET = 6,
        RPD_B_SECRET = 7,
        RPD_E_SECRET = 8,
        WaterPlant_A = 9,
        WaterPlant_B = 10,
        WaterPlant_C = 11,
        WasteWater_A = 12,
        WasteWater_B = 13,
        WasteWater_C = 14,
        WasteWater_A_SECRET = 15,
        WasteWater_B_SECRET = 16,
        WasteWater_C_SECRET = 17,
        CityArea_A = 18,
        CityArea_B = 19,
        OrphanAsylum_A = 20,
        OrphanAsylum_B = 21,
        OrphanAsylum_C = 22,
        Laboratory_A = 23,
        Laboratory_A2 = 24,
        Laboratory_A3 = 25,
        Laboratory_B = 26,
        Laboratory_B2 = 27,
        Laboratory_C = 28,
        Laboratory_D = 29,
        Laboratory_D2 = 30,
        Laboratory_D3 = 31,
        Laboratory_D4 = 32,
        FLOOR_NUM = 33,
    };
}
namespace via::effect::gpgpu::detail {
    enum class GpuEmitterShape : int64_t {
        None = 0,
        Sphere = 1,
        Cylinder = 2,
        Box = 3,
    };
}
namespace app::ropeway::gui::GimmickFilePcGuiBehavior {
    enum class StateType : int64_t {
        NONE = 0,
        SCREENSAVER = 1,
        SUCCESS = 2,
        FAILURE = 3,
        OPEN = 4,
    };
}
namespace app::ropeway::gamemastering::item {
    enum class Disposable : int64_t {
        Infinity = 0,
        SingleUse = 1,
        MultipleUse = 2,
    };
}
namespace app::ropeway::gui::NewInventoryBehavior {
    enum class GetMapState : int64_t {
        Start = 0,
        Wait = 1,
    };
}
namespace via::effect::detail {
    enum class LightingType : int64_t {
        None = 0,
        Vertex = 1,
        Vertex2x2 = 2,
        Vertex4x4 = 3,
        Vertex8x8 = 4,
        ForceWord = 0xFFFFFFFF,
    };
}
namespace app::ropeway {
    enum class GUISegmentOrder : int64_t {
        MouesCursor = 60,
        SaveIcon = 59,
        GuideIcon = 58,
        GeneralDialog = 57,
        LoadingIcon = 56,
        LoadingFade = 55,
        InterludeFade = 55,
        OverMenuFade = 54,
        MenuFade = 53,
        TitleDialog = 50,
        FullScreenMovie = 49,
        Option = 48,
        Pause = 46,
        Tutorial = 45,
        BootFlow = 44,
        Title = 43,
        GameOver = 42,
        InGameFade = 40,
        Subtitle = 38,
        EventTextureForDevelop = 37,
        SubtitleFullScreenMovie = 36,
        InventoryFront = 35,
        Purpose = 34,
        FileViewer = 30,
        InventoryCaption = 26,
        InventoryDetail = 25,
        MapFrame = 23,
        Map = 22,
        MapGrid = 21,
        MapBg = 20,
        InventorySlot = 18,
        InventoryVital = 17,
        InventoryBG = 16,
        InventoryPlayer = 15,
        SkipFade = 14,
        EventFade = 13,
        FsmFade = 10,
        Reticle = 9,
        DEFAULT_LOWER = 0,
    };
}
namespace via {
    enum class DateFormat : int64_t {
        YYYYMMDD = 0,
        DDMMYYYY = 1,
        MMDDYYYY = 2,
        Max = 3,
        Unknown = 3,
    };
}
namespace via::Frustum {
    enum class PLANE_TYPE : int64_t {
        PLANE_BOTTOM = 0,
        PLANE_TOP = 1,
        PLANE_LEFT = 2,
        PLANE_RIGHT = 3,
        PLANE_NEAR = 4,
        PLANE_FAR = 5,
    };
}
namespace app::ropeway::gimmick::CheckCondition {
    enum class CheckLogic : int64_t {
        AND = 0,
        OR = 1,
        ALL_NOT = 2,
        NOT_CHECK_FLAG = 3,
    };
}
namespace via::render {
    enum class LightingOption : int64_t {
        Legacy = 0,
        Default = 1,
        High = 2,
        Spherical = 3,
        SphericalHigh = 4,
    };
}
namespace via::hid::hmd::MorpheusDevice {
    enum class StartResult : int64_t {
        OK = 0,
        ErrorNotSupported = 1,
        ErrorNotStandby = 2,
        ErrorStartupFailed = 3,
    };
}
namespace app::ropeway::Report {
    enum class Category : int64_t {
        LevelDesign = 0,
        Timeline = 1,
        Undefined = 2,
        Max = 3,
    };
}
namespace app::ropeway::qc::QualityPerformanceBalancer {
    enum class PauseReason : int64_t {
        None = 0,
        Debug = 1,
        Event = 2,
    };
}
namespace app::ropeway::posteffect::SetPostEffectParam {
    enum class UseParamBitFlags : int64_t {
        UseChangeFogParam = 1,
        UseChangeMotionBlurParam = 2,
        UseChangeSSAOParam = 4,
        UseChangeToneMapParam = 8,
        UseChangeDOFParam = 16,
        UseChangeSSRParam = 32,
        UseChangeColorCorrectParam = 64,
        UseChangePlayerGameCameraParam = 128,
        UseChangeColorDeficiencySimulationParam = 256,
        UseChangeFilmGrainParam = 512,
        UseChangeImagePlaneParam = 1024,
        UseChangeLightShaftFilterControlParam = 2048,
        UseChangeLensDistortionParam = 4096,
        UseChangeRadialBlurParam = 8192,
        UseChangeSSSSSControlParam = 16384,
        UseChangeSoftBloomParam = 32768,
        UseChangeFFTBloomParam = 65536,
        UseChangeRenderOutput = 131072,
    };
}
namespace app::ropeway::implement::Melee {
    enum class Attack : int64_t {
        Invalid = 0xFFFFFFFF,
        High = 0,
        Low = 1,
        TerraIN_ = 2,
        Fatal = 3,
        Defense = 4,
        Stealth = 5,
    };
}
namespace app::ropeway::GimmickBattleCraneManager {
    enum class HitTarget : int64_t {
        ERROR_ = 0,
        NONE = 1,
        PL = 2,
        G2 = 3,
    };
}
namespace via::gui {
    enum class FontColorType : int64_t {
        Fill = 0,
        Vertical = 1,
    };
}
namespace app::ropeway::gimmick::action::AreaAccessSensor {
    enum class MultiAreaCheckType : int64_t {
        AND = 0,
        OR = 1,
    };
}
namespace via::effect::script::EPVExpertHitEffectData {
    enum class DIRECTIONTYPE : int64_t {
        DefenderCollisionNomal = 0,
        InverseDefenderCollisionNormal = 1,
        AttackDirection = 2,
        InverseAttackDirection = 3,
    };
}
namespace via::detail_bitset {
    enum class BIT_IMPL : int64_t {
        BIT_IMPL_32 = 0,
        BIT_IMPL_64 = 1,
        BIT_IMPL_ARRAY = 2,
    };
}
namespace via::render {
    enum class PrimitiveError : int64_t {
        NoERROR_ = 0,
        InsufficientMemory = 1,
        InvalidState = 2,
        InsufficientTexture = 3,
        InvalidOperation = 4,
    };
}
namespace via::wwise::WwiseIdNameInfo {
    enum class LanguageType : int64_t {
        None = 0,
        Japanese = 1,
        English_US = 2,
        French_France = 3,
        Italian = 4,
        German = 5,
        Spanish_SpaIN_ = 6,
        Chinese_PRC = 7,
    };
}
namespace System::Reflection {
    enum class MethodImplAttributes : int64_t {
        CodeTypeMask = 3,
        IL = 0,
        Native = 1,
        OPTIL = 2,
        Runtime = 3,
        ManagedMask = 4,
        Unmanaged = 4,
        Managed = 0,
        ForwardRef = 16,
        PreserveSig = 128,
        InternalCall = 4096,
        Synchronized = 32,
        NoInlining = 8,
        AggressiveInlining = 256,
        NoOptimization = 64,
        MaxMethodImplVal = 65535,
    };
}
namespace via::behaviortree::SelectorCallerArg {
    enum class EventType : int64_t {
        None = 0,
        EveryFrame = 1,
        ChildEnd = 2,
    };
}
namespace via {
    enum class ProjectionType : int64_t {
        PerspectiveFovRH = 0,
        OrthographicRH = 1,
        Max = 2,
    };
}
namespace via::hid::hmd::Morpheus {
    enum class VrModeStatus : int64_t {
        Unavailable = 0,
        Available = 1,
        Initializing = 2,
        Running = 3,
        Finished = 4,
    };
}
namespace app::ropeway::gui::FloorMapSt4BBehavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        Door_3_1_027 = 1,
        Door_3_1_028 = 2,
        Door_3_1_032 = 3,
        Door_3_1_033 = 4,
        Door_3_1_034 = 5,
        Door_3_1_035 = 6,
        Door_3_1_036 = 7,
        Door_3_1_007 = 8,
        Door_3_1_038 = 9,
        Door_3_1_039 = 10,
        Door_3_1_005 = 11,
        Door_3_1_004 = 12,
        Door_3_1_041 = 13,
        Door_3_1_006 = 14,
    };
}
namespace via::motion::IkMultipleDamageAction {
    enum class DamageTransition : int64_t {
        Non = 0,
        Impact = 1,
        Transition = 2,
        Damping = 3,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::DropSupportItem {
    enum class SetType : int64_t {
        On = 0,
        Off = 1,
        Keep = 2,
    };
}
namespace via::physics::System {
    enum class DirtyType : int64_t {
        RequestSetColliders = 0,
        Max = 1,
    };
}
namespace via::render::layer::Transparent {
    enum class UITargetType : int64_t {
        Default = 0,
        Max = 1,
    };
}
namespace via::render::ToneMapping {
    enum class AutoExposure : int64_t {
        Enable = 0,
        FixedEnable = 1,
        Disable = 2,
    };
}
namespace app::ropeway::fsmv2::enemy::stateaction::Em7100ActionState_ACT_LOOP_COMMON {
    enum class eLoopType : int64_t {
        LoopType_None = 0,
        LoopType_Time = 1,
        LoopType_MotionEnd = 2,
        LoopType_Custom = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickCustomJackControl {
    enum class Routine : int64_t {
        UNSET = 0,
        LOADING = 1,
        BEFORE = 2,
        READY = 3,
        GO = 4,
        AFTER = 5,
        END = 6,
    };
}
namespace via::effect::script::EnvironmentEffectManager {
    enum class RequestStatus : int64_t {
        None = 0,
        Create = 1,
    };
}
namespace app::ropeway::fsmv2::ConstActionTargetAction {
    enum class ConstModeType : int64_t {
        Once = 0,
        OnceKeep = 1,
        Keep = 2,
    };
}
namespace app::ropeway::gui::GimmickNumberLockGuiBehavior {
    enum class MaterialEnum : int64_t {
        Key0 = 0,
        Key1 = 1,
        Key2 = 2,
        Key3 = 3,
        Key4 = 4,
        Key5 = 5,
        Key6 = 6,
        Key7 = 7,
        Key8 = 8,
        Key9 = 9,
        Red = 10,
        Blue = 11,
        KeyBack = 12,
        KeyEnter = 13,
        NumMax = 14,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::NotifyRoleActionEnd {
    enum class NotifyTiming : int64_t {
        Start = 0,
        End = 1,
        FindTarget = 2,
        EndOrFindTarget = 3,
        EndWithDead = 4,
    };
}
namespace app::ropeway::EquipmentDefine {
    enum class Shortcut : int64_t {
        Up = 0,
        Down = 1,
        Left = 2,
        Right = 3,
    };
}
namespace via::fsm::StateGraph {
    enum class StateAttribute : int64_t {
        StateAttributeBitNum = 32,
    };
}
namespace app::ropeway::gui::ShortcutBehavior {
    enum class State : int64_t {
        NONE = 0,
        MAIN_ = 1,
        SUB = 2,
    };
}
namespace app::ropeway::timeline::TimelinePreparation {
    enum class ProcessState : int64_t {
        Init = 0,
        Play = 1,
        Finish = 2,
    };
}
namespace via::motion::DevelopRetarget {
    enum class Mode : int64_t {
        None = 0,
        Child = 1,
        Constraint = 2,
    };
}
namespace via::motion {
    enum class JointType : int64_t {
        Root = 0,
        Hips = 1,
        Spine = 2,
        Spine1 = 3,
        Spine2 = 4,
        Spine3 = 5,
        Neck0 = 6,
        Neck1 = 7,
        Neck2 = 8,
        Neck3 = 9,
        Head = 10,
        LeftEye = 11,
        RightEye = 12,
        LeftCollar = 13,
        LeftUpperArm = 14,
        LeftRightArm = 15,
        LeftHand = 16,
        RightCollar = 17,
        RightUpperArm = 18,
        RightRightArm = 19,
        RightHand = 20,
        LeftUpperLeg = 21,
        LeftLowerLeg = 22,
        LeftFoot = 23,
        LeftToe = 24,
        RightUpperLeg = 25,
        RightLowerLeg = 26,
        RightFoot = 27,
        RightToe = 28,
        LeftThumb0 = 29,
        LeftThumb1 = 30,
        LeftThumb2 = 31,
        LeftIndex0 = 32,
        LeftIndex1 = 33,
        LeftIndex2 = 34,
        LeftMiddle0 = 35,
        LeftMiddle1 = 36,
        LeftMiddle2 = 37,
        LeftRing0 = 38,
        LeftRing1 = 39,
        LeftRing2 = 40,
        LeftLittle0 = 41,
        LeftLittle1 = 42,
        LeftLittle2 = 43,
        RightThumb0 = 44,
        RightThumb1 = 45,
        RightThumb2 = 46,
        RightIndex0 = 47,
        RightIndex1 = 48,
        RightIndex2 = 49,
        RightMiddle0 = 50,
        RightMiddle1 = 51,
        RightMiddle2 = 52,
        RightRing0 = 53,
        RightRing1 = 54,
        RightRing2 = 55,
        RightLittle0 = 56,
        RightLittle1 = 57,
        RightLittle2 = 58,
        Count = 59,
    };
}
namespace app::ropeway::NaviMoveSupporter {
    enum class NavigationFailedType : int64_t {
        Stop = 0,
        Straight = 1,
    };
}
namespace app::ropeway::NaviMoveSupporter::PortalCorrect {
    enum class WeightType : int64_t {
        MaxDistanceSq = 0,
        MinDistanceSq = 1,
        MaxDistance = 2,
        MinDistance = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickPressed {
    enum class FourWay : int64_t {
        Z_PLUS = 0,
        X_PLUS = 1,
        Z_MINUS = 2,
        X_MINUS = 3,
        NONE = 4,
        MAX = 4,
    };
}
namespace via::effect::gpgpu::RibbonFollowCS::RibbonResource {
    enum class ComputeShaderType : int64_t {
        InitializeCS = 0,
        EmitCS = 1,
        EmitShapeOperatorCS = 2,
        EmitOnMeshContourCS = 3,
        EmitOnMeshContourCS_ColorMap = 4,
        EmitOnMeshContourCS_MaskMap = 5,
        EmitOnMeshContourCS_ColorMaskMap = 6,
        EmitOnMeshNormalContourCS = 7,
        EmitOnMeshNormalContourCS_ColorMap = 8,
        EmitOnMeshNormalContourCS_MaskMap = 9,
        EmitOnMeshNormalContourCS_ColorMaskMap = 10,
        UpdateCS = 11,
        UpdateVectorFieldCS = 12,
        UpdateSimpleVectorFieldCS = 13,
        UpdateVolumeFieldCS = 14,
        UpdateSimpleVolumeFieldCS = 15,
        UpdateMeshContourCS = 16,
        UpdateMeshContourVectorFieldCS = 17,
        UpdateMeshContourSimpleVectorFieldCS = 18,
        UpdateMeshContourVolumeFieldCS = 19,
        UpdateMeshContourSimpleVolumeFieldCS = 20,
        ParticleWarmingCS = 21,
        ParticleWarmingVectorFieldCS = 22,
        ParticleWarmingSimpleVectorFieldCS = 23,
        ParticleWarmingVolumeFieldCS = 24,
        ParticleWarmingSimpleVolumeFieldCS = 25,
        ParticleWarmingShapeOperatorCS = 26,
        ParticleWarmingVectorFieldShapeOperatorCS = 27,
        ParticleWarmingSimpleVectorFieldShapeOperatorCS = 28,
        ParticleWarmingVolumeFieldShapeOperatorCS = 29,
        ParticleWarmingSimpleVolumeFieldShapeOperatorCS = 30,
        ParticleWarmingOnMeshContourCS = 31,
        ParticleWarmingOnMeshContourCS_ColorMap = 32,
        ParticleWarmingOnMeshContourCS_MaskMap = 33,
        ParticleWarmingOnMeshContourCS_ColorMaskMap = 34,
        ParticleWarmingOnMeshContourVectorFieldCS = 35,
        ParticleWarmingOnMeshContourVectorFieldCS_ColorMap = 36,
        ParticleWarmingOnMeshContourVectorFieldCS_MaskMap = 37,
        ParticleWarmingOnMeshContourVectorFieldCS_ColorMaskMap = 38,
        ParticleWarmingOnMeshContourVolumeFieldCS = 39,
        ParticleWarmingOnMeshContourVolumeFieldCS_ColorMap = 40,
        ParticleWarmingOnMeshContourVolumeFieldCS_MaskMap = 41,
        ParticleWarmingOnMeshContourVolumeFieldCS_ColorMaskMap = 42,
        ParticleWarmingOnMeshContourSimpleVectorFieldCS = 43,
        ParticleWarmingOnMeshContourSimpleVectorFieldCS_ColorMap = 44,
        ParticleWarmingOnMeshContourSimpleVectorFieldCS_MaskMap = 45,
        ParticleWarmingOnMeshContourSimpleVectorFieldCS_ColorMaskMap = 46,
        ParticleWarmingOnMeshContourSimpleVolumeFieldCS = 47,
        ParticleWarmingOnMeshContourSimpleVolumeFieldCS_ColorMap = 48,
        ParticleWarmingOnMeshContourSimpleVolumeFieldCS_MaskMap = 49,
        ParticleWarmingOnMeshContourSimpleVolumeFieldCS_ColorMaskMap = 50,
        ParticleWarmingOnMeshNormalContourCS = 51,
        ParticleWarmingOnMeshNormalContourCS_ColorMap = 52,
        ParticleWarmingOnMeshNormalContourCS_MaskMap = 53,
        ParticleWarmingOnMeshNormalContourCS_ColorMaskMap = 54,
        ParticleWarmingOnMeshNormalContourVectorFieldCS = 55,
        ParticleWarmingOnMeshNormalContourVectorFieldCS_ColorMap = 56,
        ParticleWarmingOnMeshNormalContourVectorFieldCS_MaskMap = 57,
        ParticleWarmingOnMeshNormalContourVectorFieldCS_ColorMaskMap = 58,
        ParticleWarmingOnMeshNormalContourVolumeFieldCS = 59,
        ParticleWarmingOnMeshNormalContourVolumeFieldCS_ColorMap = 60,
        ParticleWarmingOnMeshNormalContourVolumeFieldCS_MaskMap = 61,
        ParticleWarmingOnMeshNormalContourVolumeFieldCS_ColorMaskMap = 62,
        ParticleWarmingOnMeshNormalContourSimpleVectorFieldCS = 63,
        ParticleWarmingOnMeshNormalContourSimpleVectorFieldCS_ColorMap = 64,
        ParticleWarmingOnMeshNormalContourSimpleVectorFieldCS_MaskMap = 65,
        ParticleWarmingOnMeshNormalContourSimpleVectorFieldCS_ColorMaskMap = 66,
        ParticleWarmingOnMeshNormalContourSimpleVolumeFieldCS = 67,
        ParticleWarmingOnMeshNormalContourSimpleVolumeFieldCS_ColorMap = 68,
        ParticleWarmingOnMeshNormalContourSimpleVolumeFieldCS_MaskMap = 69,
        ParticleWarmingOnMeshNormalContourSimpleVolumeFieldCS_ColorMaskMap = 70,
        LightingCS = 71,
        ParticleCollider = 72,
        MaxType = 73,
    };
}
namespace via::os::http_client {
    enum class Security : int64_t {
        IgnoreUnknownCA = 1,
        IgnoreCertCnInvalid = 2,
        IgnoreCertDateInvalid = 4,
        IgnoreCertWrongUsage = 8,
        AllowRejectedCert = 16,
        IgnoreValidationCache = 32,
    };
}
namespace app::ropeway::gimmick::option::EnemyConditionCheckSettings::EnemyConditionCheckParam {
    enum class CameraCheckType : int64_t {
        CameraIN_ = 0,
        CameraOver = 1,
    };
}
namespace via {
    enum class bitset_assign_option : int64_t {
        bits = 0,
        pos = 1,
    };
}
namespace app::ropeway::gui::RogueInventoryDetailBehavior {
    enum class Mode : int64_t {
        Invalid = 0,
        Detail = 1,
        PickUpNormal = 2,
        PickUpFirst = 3,
        PickUpFirstCombination = 4,
        PickUpFirstCombinationDemo = 5,
        PickUpMap = 6,
    };
}
namespace app::ropeway::enemy::em3000::fsmv2::action::Em3000FsmAction_ActionCameraPlay {
    enum class CameraPatternType : int64_t {
        HOLD_START = 0,
        HOLD_START_BACK = 1,
        HOLD_KILL = 2,
        HOLD_PUSH = 3,
        HOLD_GRENADE = 4,
        HOLD_KNIFE = 5,
    };
}
namespace app::ropeway::gui::TitleRBehavior {
    enum class DecidedResult : int64_t {
        ROGUE = 0,
        THE_4TH = 1,
        TOFU = 2,
        L_A = 3,
        L_B = 4,
        L_C = 5,
        L_D = 6,
        MAX = 7,
        CANCEL = 8,
    };
}
namespace app::ropeway::enemy::em4000::tracks::Em4000HoldDamageTrack {
    enum class HoldDamageType : int64_t {
        HOLD = 0,
        HOLD_KILL = 1,
    };
}
namespace via::hid {
    enum class AccountPickerState : int64_t {
        NULL_ = 0,
        Idle = 1,
        Executing = 2,
        PostProc = 3,
    };
}
namespace app::ropeway::EmDamagedSeDefine {
    enum class Loudness : int64_t {
        Low = 0,
        Middle = 1,
        High = 2,
    };
}
namespace via::CoreEntry {
    enum class VrDeviceRequirement : int64_t {
        No = 0,
        Maybe = 1,
        Require = 2,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class Eat : int64_t {
        EAT_CORPSE_00 = 0,
        EAT_CORPSE_01 = 1,
        EAT_CORPSE_02 = 2,
        EAT_00 = 3,
    };
}
namespace app::ropeway::weapon::shell::ShellBase {
    enum class BombShellStatusInformation : int64_t {
        NoSuspend = 0,
        Update = 1,
        LateUpdate = 2,
    };
}
namespace via::motion::tree::BaseGamePadStickNode {
    enum class StickKind : int64_t {
        Left = 0,
        Right = 1,
    };
}
namespace app::ropeway::fsmv2 {
    enum class ExecutePhase : int64_t {
        START = 1,
        UPDATE = 2,
        END = 4,
        SEQUENCE = 8,
    };
}
namespace app::ropeway::gimmick::option::ChangeMapSpaceInfoSettings::Param {
    enum class Command : int64_t {
        Invalid = 0,
        Add = 1,
        Remove = 2,
    };
}
namespace app::ropeway::OptionManager {
    enum class AudioSpeakerType : int64_t {
        SURROUND = 0,
        TV = 1,
        HEADPHONE = 2,
        ATMOS = 3,
    };
}
namespace app::ropeway::gui::FileBehavior {
    enum class ViewMode : int64_t {
        NONE = 0,
        PREVIEW = 1,
        DETAIL = 2,
    };
}
namespace app::ropeway::gimmick::GimmickConfiscee::ParamMotion {
    enum class ParamType : int64_t {
        UNSET = 0,
        DOOR = 1,
    };
}
namespace app::ropeway::gui::FloorMapSt13Behavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_1_610_0 = 1,
        Room_1_611_0 = 2,
        Room_1_612_0 = 3,
        Room_1_613_0 = 4,
        Room_1_614_0 = 5,
    };
}
namespace app::ropeway::TagList {
    enum class Symbol : int64_t {
        Invalid = 0,
        TerraIN_ = 1,
        Gimmick = 2,
        Player = 3,
        Enemy = 4,
        NPC = 5,
        Camera = 6,
        GUI = 7,
        Props = 8,
        Item = 9,
        Gate = 10,
        Actor = 11,
        Location = 12,
        Masters = 13,
        Effect = 14,
        GimmickLadderPos = 15,
        GimmickJumpDownPos = 16,
    };
}
namespace app::ropeway::gimmick::action::GimmickCircuitPuzzle {
    enum class ActionResult : int64_t {
        NOT_YET = 0,
        END = 1,
        FAILED = 2,
    };
}
namespace via {
    enum class SystemServiceCaps : int64_t {
        None = 0,
        Language = 1,
        DateFormat = 2,
        TimeFormat = 4,
        TimeZoneOffset = 8,
        SummerTime = 16,
        SystemUiOverlay = 32,
        BackgroundExecution = 64,
        ApplicationSuspend = 128,
        ApplicationResume = 256,
        GetDisplaySafeAreaRatio = 512,
        SplashScreenControl = 1024,
        ScreenSaverControl = 2048,
        ApplicationParameter = 4096,
        SkuFlag = 8192,
        ResetVrPosition = 16384,
        OpenShareMenu = 32768,
        VideoRecorder = 65536,
        ApplicationActivate = 131072,
        ApplicationDeactivate = 262144,
        Country = 524288,
        AccountPicker = 1048576,
    };
}
namespace via::render::RenderConfig {
    enum class Quality : int64_t {
        LOWEST = 0,
        LOW = 1,
        STANDARD = 2,
        HIGH = 3,
        HIGHEST = 4,
        NONE = 5,
    };
}
namespace via::hid::VrTrackerResultData {
    enum class Validation : int64_t {
        Active = 0,
        Inactive = 1,
        Unknown = 2,
    };
}
namespace app::ropeway::enemy::em6300::MotionPattern {
    enum class WalkTurn : int64_t {
        L90 = 0,
        R90 = 1,
        L180 = 2,
        R180 = 3,
    };
}
namespace via::clr {
    enum class FieldFlag : int64_t {
        FieldAccessMask = 7,
        PrivateScope = 0,
        Private = 1,
        FamANDAssem = 2,
        Assembly = 3,
        Family = 4,
        FamORAssem = 5,
        Public = 6,
        Static = 16,
        InitOnly = 32,
        Literal = 64,
        NotSerialized = 128,
        HasFieldRVA = 256,
        SpecialName = 512,
        RTSpecialName = 1024,
        Pointer = 2048,
        HasFieldMarshal = 4096,
        PInvokeImpl = 8192,
        ExposeMember = 16384,
        HasDefault = 32768,
        ReservedMask = 38144,
    };
}
namespace app::ropeway::OptionManager {
    enum class ResultTextureID : int64_t {
        NONE = 0,
        ui0441_graphic00_00_IM = 1,
        ui0441_graphic00_01_IM = 2,
        ui0441_graphic00_02_IM = 3,
        ui0441_graphic01_00_IM = 4,
        ui0441_graphic01_01_IM = 5,
        ui0441_graphic01_02_IM = 6,
        ui0441_graphic01_03_IM = 7,
        ui0441_graphic01_04_IM = 8,
        ui0441_graphic01_05_IM = 9,
        ui0441_graphic02_00_IM = 10,
        ui0441_graphic02_01_IM = 11,
        ui0441_graphic02_02_IM = 12,
        ui0441_graphic02_03_IM = 13,
        ui0441_graphic02_04_IM = 14,
        ui0441_graphic03_00_IM = 15,
        ui0441_graphic03_01_IM = 16,
        ui0441_graphic03_02_IM = 17,
        ui0441_graphic03_03_IM = 18,
        ui0441_graphic03_04_IM = 19,
        ui0441_graphic03_05_IM = 20,
        ui0441_graphic09_00_IM = 21,
        ui0441_graphic09_01_IM = 22,
        ui0441_graphic09_02_IM = 23,
        ui0441_graphic09_03_IM = 24,
        ui0441_graphic09_04_IM = 25,
        ui0441_graphic11_00_IM = 26,
        ui0441_graphic11_01_IM = 27,
        ui0441_graphic13_00_IM = 28,
        ui0441_graphic13_01_IM = 29,
        ui0441_graphic14_00_IM = 30,
        ui0441_graphic14_01_IM = 31,
        ui0441_graphic16_00_IM = 32,
        ui0441_graphic16_01_IM = 33,
        ui0441_graphic17_00_IM = 34,
        ui0441_graphic17_01_IM = 35,
        ui0441_graphic18_00_IM = 36,
        ui0441_graphic18_01_IM = 37,
        ui0441_graphic19_00_IM = 38,
        ui0441_graphic19_01_IM = 39,
        ui0441_graphic19_02_IM = 40,
        ui0441_graphic20_00_IM = 41,
        ui0441_graphic20_01_IM = 42,
        ui0441_graphic04_00_IM = 43,
        ui0441_graphic04_01_IM = 44,
        ui0441_graphic04_02_IM = 45,
        ui0441_graphic04_03_IM = 46,
        ui0441_graphic06_00_IM = 47,
        ui0441_graphic06_01_IM = 48,
        ui0441_graphic06_02_IM = 49,
        ui0441_graphic06_03_IM = 50,
        ui0441_graphic06_04_IM = 51,
        ui0441_graphic07_00_IM = 52,
        ui0441_graphic07_01_IM = 53,
        ui0441_graphic10_00_IM = 54,
        ui0441_graphic10_01_IM = 55,
        ui0441_graphic12_00_IM = 56,
        ui0441_graphic12_01_IM = 57,
        ui0441_graphic12_02_IM = 58,
        ui0441_graphic12_03_IM = 59,
        ui0441_graphic12_04_IM = 60,
        ui0441_graphic15_00_IM = 61,
        ui0441_graphic15_01_IM = 62,
        ui0441_graphic15_02_IM = 63,
        ui0441_graphic15_03_IM = 64,
        ui0441_graphic02_05_IM = 65,
        ui0441_graphic02_06_IM = 66,
        ui0441_graphic02_07_IM = 67,
        ui0441_graphic21_00_IM = 68,
        ui0441_graphic21_01_IM = 69,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class Accessory : int64_t {
        Invalid = 0xFFFFFFFF,
        Ac0000 = 0,
        Ac0001 = 1,
        Ac0002 = 2,
        Ac0010 = 3,
        Ac0011 = 4,
        Ac0012 = 5,
        Ac0013 = 6,
        Ac0014 = 7,
        Ac0015 = 8,
        Ac0016 = 9,
        Ac0017 = 10,
        Ac0018 = 11,
        Ac0019 = 12,
        Ac0020 = 13,
        Ac0021 = 14,
        Ac0022 = 15,
        Ac0030 = 16,
        Ac0031 = 17,
        Ac0032 = 18,
        Ac0033 = 19,
    };
}
namespace app::ropeway::EnvironmentStandbyManager {
    enum class TargetedReason : int64_t {
        None = 0,
        PlayerActive = 1,
        DirectRequested = 2,
        LastValidRequest = 3,
    };
}
namespace via::hid::camera {
    enum class PlayStationCameraStartResult : int64_t {
        OK = 1,
        Started = 1,
        ErrorConfigType = 2147483649,
        ErrorInternal = 2147487744,
    };
}
namespace app::ropeway::gui::RogueFlyTextBehavior {
    enum class State : int64_t {
        INVALID = 0xFFFFFFFF,
        INIT = 0,
        UPDATE = 1,
        FINISHED = 2,
    };
}
namespace via::motion::TransitionData {
    enum class EndType : int64_t {
        None = 0,
        EndOfMotion = 1,
        ExitFrame = 2,
        ExitFrameFromEnd = 3,
    };
}
namespace via::physics {
    enum class TriangleVoronoiId : int64_t {
        Internal = 0,
        Edge01 = 1,
        Edge20 = 2,
        V0 = 3,
        Edge12 = 4,
        V1 = 5,
        V2 = 6,
        Max = 7,
    };
}
namespace app::ropeway::OptionKeyBind {
    enum class State : int64_t {
        Selection = 0,
        KeyReceiving = 1,
        EmptyCutIN_ = 2,
        ErrorCutIN_ = 3,
        DefaultCutIN_ = 4,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class TerritoryChooseType : int64_t {
        NONE = 0,
        MY_ZONE = 1,
        PARENT_ZONE = 2,
    };
}
namespace via::navigation::NavigationSurface {
    enum class PortalOffsetDirectionType : int64_t {
        InOut = 0,
        LeftRight = 1,
    };
}
namespace via::Triangle {
    enum class NGLevel : int64_t {
        None = 0,
        EdgeShort = 1,
        LS = 2,
        Point = 3,
        EdgeLong = 4,
        CantCalcGID = 5,
        NonFinite = 6,
        Assert = 7,
        Default = 0,
    };
}
namespace app::ropeway::gui::RogueAccessoryBehavior {
    enum class ChangeState : int64_t {
        NONE = 0,
        REQUEST = 1,
        CHANGE = 2,
    };
}
namespace app::ropeway::enemy::em7300::Em7300Think {
    enum class WeakEyeEnum : int64_t {
        _00 = 0,
        _01 = 1,
        _02 = 2,
        _03 = 3,
        _04 = 4,
        _05 = 5,
        _06 = 6,
        _07 = 7,
        _08 = 8,
        _09 = 9,
        _10 = 10,
    };
}
namespace app::ropeway::weapon::generator::BombEmberGenerator {
    enum class State : int64_t {
        INVALID = 0,
        READY = 1,
        GENERATING = 2,
        GENERATED = 3,
    };
}
namespace via::wwise::WwiseListener {
    enum class ListenerIndex : int64_t {
        ListenerIndex_0 = 0,
        ListenerIndex_1 = 1,
        ListenerIndex_2 = 2,
        ListenerIndex_3 = 3,
        ListenerIndex_4 = 4,
        ListenerIndex_5 = 5,
        ListenerIndex_6 = 6,
        ListenerIndex_7 = 7,
    };
}
namespace via::hid::VrTrackerResultData {
    enum class Status : int64_t {
        NotStarted = 0,
        Tracking = 1,
        NotTracking = 2,
        Calibrating = 3,
        ErrorGetResultDataFailed = 0xFFFFFFFF,
    };
}
namespace via::render::Stamp {
    enum class StampBlendMethod : int64_t {
        Opaque = 0,
        Add = 1,
        AddBlend = 2,
        AlphaBlend = 3,
        SubBlend = 4,
        Maximum = 5,
    };
}
namespace System::Reflection {
    enum class GenericParameterAttributes : int64_t {
        None = 0,
        VarianceMask = 3,
        Covariant = 1,
        Contravariant = 2,
        SpecialConstraintMask = 28,
        ReferenceTypeConstraint = 4,
        NotNullableValueTypeConstraint = 8,
        DefaultConstructorConstraint = 16,
    };
}
namespace via::os {
    enum class FileAttr : int64_t {
        Read = 0,
        Write = 1,
        ReadWrite = 2,
    };
}
namespace app::ropeway::gamemastering::RogueResultFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        WAIT_CLEAR_MOVIE_END = 1,
        WAIT_GUI_CREATION = 2,
        UPDATE = 3,
        WAIT_FADE = 4,
        FINALIZE = 5,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace app::ropeway::CollisionDefine {
    enum class HitGroup : int64_t {
        Empty = 0,
        Player = 1,
        Enemy = 2,
        Gimmick = 3,
    };
}
namespace app::ropeway::gimmick::GimmickWiringNodeManager {
    enum class ColorType : int64_t {
        UNSET = 0,
        NORMAL = 1,
        NOIR = 2,
    };
}
namespace app::ropeway::weapon::generator::BombGeneratorUserDataBase {
    enum class BombGrenadeGenerateSetting : int64_t {
        S = 0,
        M = 1,
        L = 2,
    };
}
namespace app::ropeway::fsmv2::enemy::em7100 {
    enum class THINK_TAG : int64_t {
        DISABLE_EYE_LOOK_AT = 2085186476,
        DISABLE_EYE_MOTION = 2526184245,
        RANK_MOVE_SPEED = 3648666295,
        DISABLE_CHARACTER_CONTROLLER = 321479229,
    };
}
namespace app::ropeway {
    enum class BehaviorTreeActionCategory : int64_t {
        Invalid = 0xFFFFFFFF,
        BehaviorTree = 0,
        MotionFsm = 1,
        MotionJackFsm = 2,
    };
}
namespace via::Triangle {
    enum class VoronoiId : int64_t {
        Internal = 0,
        Edge01 = 1,
        Edge20 = 2,
        V0 = 3,
        Edge12 = 4,
        V1 = 5,
        V2 = 6,
    };
}
namespace app::ropeway::IkAdjustLimb {
    enum class LimbIndex : int64_t {
        LIMB_00 = 0,
        LIMB_01 = 1,
        LIMB_02 = 2,
        LIMB_03 = 3,
        LIMB_04 = 4,
        LIMB_05 = 5,
        LIMB_06 = 6,
        LIMB_07 = 7,
    };
}
namespace rapidxml {
    enum class node_type : int64_t {
        node_document = 0,
        node_element = 1,
        node_data = 2,
        node_cdata = 3,
        node_comment = 4,
        node_declaration = 5,
        node_doctype = 6,
        node_pi = 7,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class DamageReaction : int64_t {
        None = 1,
        NoReaction = 2,
        Stun = 4,
        Knockback = 8,
        Down = 16,
        Burst = 32,
        Cut = 64,
        Acid = 128,
        Burn = 256,
        Flash = 512,
        Bind = 1024,
        Shock = 2048,
        Knife = 4096,
        Tangle = 8192,
        Scatter = 16384,
        OneKill = 32768,
        SparkBullet = 65536,
        SparkSmall = 131072,
        SparkLarge = 262144,
        GrenadeBullet = 524288,
        GrenadeBlow = 1048576,
        Unique00 = 1073741824,
        Unique01 = 2147483648,
    };
}
namespace app::ropeway::gui::RopewayGimmickAttachmentGUI {
    enum class ResultState : int64_t {
        NotExecute = 0,
        Thinking = 1,
        Incorrect = 2,
        Satisfy = 3,
        Canceled = 4,
    };
}
namespace via::effect::lensflare {
    enum class ResourceType : int64_t {
        Handle = 0,
        Stream = 1,
    };
}
namespace app::ropeway::MansionManager {
    enum class AreaFoundReason : int64_t {
        NotFound = 0,
        FoundByTerraIN_ = 1,
        FoundBySameTerraIN_ = 2,
        ApproximateByLoadCollision = 3,
        ApproximateByStableInfo = 4,
        ApproximateByAreaInfo = 5,
    };
}
namespace via::render::streaming_detail {
    enum class StreamingOption : int64_t {
        Protect = 1,
    };
}
namespace app::ropeway::OperatorDefine::ComparisonOparator {
    enum class Type : int64_t {
        Equal = 0,
        NotEqual = 1,
        Less = 2,
        LessEq = 3,
        Greater = 4,
        GreaterEq = 5,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class Vital : int64_t {
        FINE = 0,
        CAUTION = 1,
        DANGER = 2,
        POISON = 3,
        DEAD = 4,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class Idle : int64_t {
        IDLE = 0,
        IDLE_LOOK_UP = 1,
    };
}
namespace app::ropeway::EnemyAttackSensor {
    enum class SensorType : int64_t {
        Type_00 = 0,
        Type_01 = 1,
        Type_02 = 2,
        Type_03 = 3,
    };
}
namespace app::ropeway::gamemastering::ResetGameFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        FADE_WAIT = 1,
        RESET_PROCESS = 2,
        RESET_WAIT = 3,
        CLEAR_MANAGERS = 4,
        FINALIZE = 5,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace app::ropeway::fsmv2::TraceVariableAction {
    enum class LogType : int64_t {
        Info = 0,
        Warning = 1,
        ERROR_ = 2,
    };
}
namespace app::ropeway::CharacterHandler {
    enum class RagdollControlType : int64_t {
        None = 0,
        RigidBodySet = 1,
        Ragdoll = 2,
        RagdollController = 3,
    };
}
namespace app::ropeway::gimmick::action::InteractManager {
    enum class ButtonType : int64_t {
        ACTION = 0,
        ITEM = 1,
        DASH = 2,
        WALK = 3,
        PUSH_XP = 4,
        PUSH_XM = 5,
        PUSH_ZP = 6,
        PUSH_ZM = 7,
        PUSH_IN_ = 8,
        PUSH_OUT = 9,
        PUSH_ANY = 10,
        DISABLE = 11,
        ACTION_INVISIBLE = 12,
        MAX = 13,
        INVALID = 0xFFFFFFFF,
    };
}
namespace app::ropeway::enemy::em3000::Sick {
    enum class Pattern : int64_t {
        Pattern1 = 0,
        Pattern2 = 1,
        Pattern3 = 2,
    };
}
namespace via::render::FakeLensflare {
    enum class ResolutionLevel : int64_t {
        Full = 0,
        Half = 1,
        Quarter = 2,
    };
}
namespace via::gui {
    enum class TypingCommand : int64_t {
        Resume = 0,
        Pause = 1,
        End = 2,
        Restart = 3,
    };
}
namespace app::ropeway::gui::RogueActivityLogBehavior {
    enum class State : int64_t {
        INVALID = 0xFFFFFFFF,
        DISABLE = 0,
        INIT = 1,
        WAIT = 2,
        PRE_UPDATE = 3,
        UPDATE = 4,
        COMPLETE = 5,
    };
}
namespace via::geometry {
    enum class BrushPrimitive : int64_t {
        Box = 0,
        Cylinder = 1,
        Cone = 2,
        Sphere = 3,
        Stairs = 4,
        Slope = 5,
        Floor = 6,
        ImagePlane = 7,
    };
}
namespace app::ropeway::enemy::EnemyDropPartsBase {
    enum class DropPartsStatus : int64_t {
        Invalid = 0xFFFFFFFF,
        Standby = 0,
        DynamicStandby = 1,
        Dynamic = 2,
        Suicide = 3,
    };
}
namespace via::render::detail {
    enum class CsPriority : int64_t {
        CSUpdate_0 = 0,
        CSUpdate_1 = 16,
        CSUpdate_2 = 32,
    };
}
namespace app::ropeway::gui::RogueInventorySlotBehavior {
    enum class DecideType : int64_t {
        NORMAL = 0,
        USE = 1,
        EQUIP = 2,
        EQUIP_OFF = 3,
        CUSTOM = 4,
        CUSTOM_OFF = 5,
        SHORTCUT = 6,
        COMBINE_CHOOSE = 7,
        COMBINE_DECIDE = 8,
        EXCHANGE = 9,
        GET_ITEM = 10,
        GO_DETAIL = 11,
        CHANGE_ID = 12,
        GO_PUZZLE = 13,
        OPEN_DIALOG = 14,
        PUTAWAY = 15,
        PICKUP = 16,
    };
}
namespace app::ropeway::enemy::em7000::Em7000Define {
    enum class SetType : int64_t {
        SetType_Defalut = 0,
        SetType_Leon = 1,
        SetType_Claire = 2,
        SetType_Rogue = 3,
    };
}
namespace via::os {
    enum class ThreadState : int64_t {
        Ready = 0,
        Executing = 1,
        Running = 2,
        Suspended = 3,
    };
}
namespace app::ropeway::enemy::em6200::MotionPattern {
    enum class Provoke : int64_t {
        CrackingNeck = 0,
        CrackingNeck2 = 1,
        CrackingHand = 2,
        Num = 3,
    };
}
namespace app::ropeway::gui::RogueInventoryDetailBehavior {
    enum class GuideType : int64_t {
        NoDisp = 0,
        Detail = 1,
        Select = 2,
        Select2 = 3,
        PortableSafe = 4,
        Oscilloscope = 5,
        Next = 6,
    };
}
namespace app::ropeway::enemy::em6000::fsmv2::action::Em6000FsmAction_Conv {
    enum class STATE : int64_t {
        SetTimer = 0,
        FlagOn = 1,
    };
}
namespace app::ropeway::behaviortree::condition::survivor::npc::NpcBtCondition_CheckRay {
    enum class CheckType : int64_t {
        Pass = 0,
        Fail = 1,
    };
}
namespace via::eq::Function {
    enum class Type : int64_t {
        SIN_ = 0,
        Cos = 1,
        ASIN_ = 2,
        ACos = 3,
        Floor = 4,
        Ceil = 5,
        Log = 6,
        Log10 = 7,
        Exp = 8,
        Abs = 9,
        Saturate = 10,
    };
}
namespace via::render {
    enum class ShadowFilter : int64_t {
        Custom = 0,
        Fast = 1,
        Default = 2,
    };
}
namespace via::physics {
    enum class ForceFillMode : int64_t {
        None = 0,
        Solid = 1,
        WireFrame = 2,
    };
}
namespace via::motion::IkLeg {
    enum class FootLockMode : int64_t {
        Auto = 0,
        SemiAuto = 1,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class TargetPriority : int64_t {
        Priority_0 = 0,
        Priority_1 = 1,
        Priority_2 = 2,
        Priority_3 = 3,
        Priority_4 = 4,
        Priority_5 = 5,
        Priority_6 = 6,
        Priority_7 = 7,
        Hide = 8,
    };
}
namespace app::ropeway::weapon::shell::ShellDefine {
    enum class ExShellTypes : int64_t {
        Default = 0,
        Ex = 1,
    };
}
namespace via::render::CaptureToTexture {
    enum class ReplayCapturedTarget : int64_t {
        Default = 0,
        DepthOnly = 1,
        ColorOnly = 2,
    };
}
namespace app::ropeway::gui::GUIMaster {
    enum class GuiState : int64_t {
        NONE = 0,
        INVENTORY = 1,
        PAUSE = 2,
        EVENT_PAUSE = 3,
        MAX = 4,
    };
}
namespace app::ropeway::gamemastering::SafeLoadManager {
    enum class LoadWaitKind : int64_t {
        EnvArea = 0,
        Event = 1,
        Undefine = 2,
    };
}
namespace app::ropeway::camera::ControlMotionArg {
    enum class ActionCallType : int64_t {
        START = 0,
        UPDATE = 1,
        END = 2,
    };
}
namespace app::ropeway::GrandEntry {
    enum class EntryType : int64_t {
        Develop = 0,
        Production = 1,
    };
}
namespace app::ropeway::gimmick::action::TriggerAreaHit {
    enum class TimingType : int64_t {
        ON_CONTACT = 0,
        ON_OVERLAPPING = 1,
        ON_SEPARATE = 2,
    };
}
namespace via::gui {
    enum class ListScrollDirection : int64_t {
        None = 0,
        Prev = 1,
        Next = 2,
    };
}
namespace app::ropeway::implement::Arm {
    enum class Ability : int64_t {
        Invalid = 0xFFFFFFFF,
        Damage = 0,
        Wince = 1,
        Break = 2,
        Critical = 3,
        FitCritical = 4,
        Slur = 5,
        SpreadAngle = 6,
        SpreadValue = 7,
        RemainingNumber = 8,
        PenetrationNumber = 9,
        RecoilLevel = 10,
        ReticleLevel = 11,
    };
}
namespace app::ropeway {
    enum class IkLockEnableOption : int64_t {
        ADJUSTING_TERRAIN_ = 0,
        PRESSING = 1,
        INTERPOLATING_MOTION = 2,
        PREVENTERING_PENETRATE = 3,
    };
}
namespace via::clr {
    enum class EnumI8 : int64_t {
        Dummy = 0,
    };
}
namespace app::ropeway::gimmick::option::CheckRankSettings::Param {
    enum class CheckLogic : int64_t {
        Equal = 0,
        Greater = 1,
        Less = 2,
        GreaterOrEqual = 3,
        LessOrEqual = 4,
    };
}
namespace app::ropeway::gui::NewInventoryBehavior {
    enum class GuideType : int64_t {
        NoDisp = 0,
        Map = 1,
        File = 2,
    };
}
namespace via::gui {
    enum class MaskType : int64_t {
        Target = 0,
        NonTarget = 1,
        Mask = 2,
        MaskTop = 3,
        MaskTopMost = 4,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class ENEMY_GROUND_STATE : int64_t {
        STAND = 0,
        JUMP = 1,
        WALL = 2,
        CEILING = 3,
    };
}
namespace via::navigation::map::NodeObject {
    enum class NodeStatus : int64_t {
        NodeStatus_Enabled = 0,
        NodeStatus_DivideReserved = 1,
        NodeStatus_Dividing = 2,
        NodeStatus_Extended = 3,
        NodeStatus_Unload = 4,
        NodeStatus_05 = 5,
        NodeStatus_06 = 6,
        NodeStatus_07 = 7,
        NodeStatus_Length = 8,
    };
}
namespace app::ropeway::navigation::DirectLinkEdge {
    enum class EdgeKind : int64_t {
        Upper = 0,
        Lower = 1,
        Invalid = 2,
    };
}
namespace app::ropeway::gui::FloorMapSt43Behavior {
    enum class BgObjectId : int64_t {
        BgObject_invalid = 0,
    };
}
namespace app::ropeway::gimmick::action::GimmickBody {
    enum class SuspendType : int64_t {
        DISABLE_MAP = 2,
        WAIT_REGISTER = 3,
    };
}
namespace app::ropeway::gui::FloorMapSt4CBehavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_4_750_0a = 1,
        Room_4_750_0b = 2,
        Room_4_751_0a = 3,
        Room_4_751_0b = 4,
        Room_4_751_0c = 5,
        Room_4_751_0d = 6,
        Room_4_751_0e = 7,
    };
}
namespace via::effect::script::EPVExpertPhysicsData {
    enum class TriggerTypeEnum : int64_t {
        Contact = 0,
        Slide = 1,
    };
}
namespace app::ropeway::gimmick::option::MovePositionSettings {
    enum class MoveObjectType : int64_t {
        Survivor = 0,
        Enemy = 1,
        Object = 2,
    };
}
namespace System::Collections::Generic {
    enum class NodeColor : int64_t {
        Black = 0,
        Red = 1,
    };
}
namespace app::ropeway::gui::FloorMapSt3DBehavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        Door_6_1_035 = 1,
        Door_6_1_036 = 2,
    };
}
namespace via::gui {
    enum class MouseSelectType : int64_t {
        None = 0,
        MouseOver = 1,
        LeftClick = 2,
    };
}
namespace app::ropeway::enemy::em7100::Em7100Ceiling {
    enum class State : int64_t {
        Default = 0,
        Break = 1,
    };
}
namespace app::ropeway::Em4100Define {
    enum class BtActionStatus : int64_t {
        ACT_IDLE = 0,
        ACT_IDLE_1 = 1,
        ACT_REACTION_0 = 2,
        ACT_REACTION_1 = 3,
        ACT_PIVOT_TURN_L90 = 4,
        ACT_PIVOT_TURN_R90 = 5,
        ACT_PIVOT_TURN_L180 = 6,
        ACT_PIVOT_TURN_R180 = 7,
        ACT_FLY = 8,
        ACT_RUN_FLY = 9,
        ACT_GLIDE = 10,
        ACT_FLAP = 11,
        ACT_RISE = 12,
        ACT_LAND = 13,
        ACT_MOVE = 14,
        ACT_STANDUP_FROM_DOWN = 15,
        ACT_CITY_FLY = 16,
        ATK_ATTACK = 2000,
        DMG_DAMAGE = 3000,
        DIE_NORMAL = 5000,
    };
}
namespace app::ropeway::ReNetData::BreakPartsData {
    enum class BreakPartsType : int64_t {
        Head = 0,
        Body = 1,
        RightArm = 2,
        LeftArm = 3,
        RightLeg = 4,
        LeftLeg = 5,
    };
}
namespace via {
    enum class FsmCategory : int64_t {
        None = 0,
        Fsm = 1,
        Mot = 2,
        Auto = 4,
        BehaviorTree = 8,
    };
}
namespace app::ropeway::weapon::shell::ShellBase{
    enum class BulletShellStatusInformation__ :  int64_t {
        NoSuspend = 0,
        Update = 1,
        LateUpdate = 2,
    };
}
namespace via::navigation::FailReport {
    enum class FailAttribute : int64_t {
        DestPosNotSpecified = 0,
        StartNodeNotFound = 1,
        DestNodeNotFound = 2,
        PathNotFound = 3,
        PathfindInterrupt = 4,
        FailAttributeNum = 5,
    };
}
namespace app::ropeway::camera {
    enum class CameraPlayStatus : int64_t {
        INVALID = 0,
        BUSY = 1,
        SWITCHING = 2,
    };
}
namespace via::render {
    enum class DebugSeverity : int64_t {
        None = 0,
        Level0 = 1,
        Level1 = 2,
        Level2 = 3,
    };
}
namespace via::motion::SubExJointRemapOutput {
    enum class CalculateMode : int64_t {
        Sum = 0,
        Average = 1,
    };
}
namespace app::ropeway::effect::script::helper::EffectMaterialGroupHelper {
    enum class MaterialGroup : int64_t {
        g_otherA = 0,
        g_soil = 1,
        g_stone = 2,
        g_wood = 3,
        g_iron = 4,
        g_glass = 5,
        g_meat = 6,
        g_mud = 7,
        g_shallowwater = 8,
        g_stickywater = 9,
        g_deepwater = 10,
        g_wethard = 11,
    };
}
namespace via::clr {
    enum class EnumU8 : int64_t {
        Dummy = 0,
    };
}
namespace app::ropeway::ReNet::PostRequest {
    enum class ResponseType : int64_t {
        PostSucceed = 0,
        PostFailed = 1,
    };
}
namespace via::effect::detail {
    enum class VelocityType : int64_t {
        Direction = 0,
        Normal = 1,
        Radial = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickClockGearControl {
    enum class GearPattern : int64_t {
        NONE = 0,
        TOP_SMALL = 1,
        TOP_LARGE = 2,
        BOTTOM_SMALL = 3,
        BOTTOM_LARGE = 4,
    };
}
namespace app::ropeway::gui::GimmickInfo {
    enum class IconType : int64_t {
        Fixed = 0,
        Variable = 1,
    };
}
namespace app::ropeway::enemy::em5000::MotionPattern {
    enum class WalkTotter : int64_t {
        TYPE_A = 0,
        TYPE_B = 1,
        TYPE_C = 2,
    };
}
namespace System {
    enum class StringComparison : int64_t {
        CurrentCulture = 0,
        CurrentCultureIgnoreCase = 1,
        InvariantCulture = 2,
        InvariantCultureIgnoreCase = 3,
        Ordinal = 4,
        OrdinalIgnoreCase = 5,
    };
}
namespace via::hid::virtualKeyboard::nsw {
    enum class InvalidButtonFlag : int64_t {
        None = 0,
        AnalogStickL = 2,
        AnalogStickR = 4,
        ZL = 8,
        ZR = 16,
    };
}
namespace app::ropeway::AssetPackagePair {
    enum class EnemyType : int64_t {
        ZOMBIE = 0,
        EM3000 = 1,
        EM4000 = 2,
        EM4100 = 3,
        EM4400 = 4,
        EM5000 = 5,
        EM6000 = 6,
        EM6100 = 7,
        EM6200 = 8,
        EM6300 = 9,
        EM7000 = 10,
        EM7100 = 11,
        EM7110 = 12,
        EM7200 = 13,
        EM7300 = 14,
        EM7400 = 15,
        EM9000 = 16,
    };
}
namespace app::ropeway::ScenarioDefine {
    enum class ScenarioNo : int64_t {
        S01_0000 = 0,
        S01_0100 = 1,
        S01_0200 = 2,
        S01_0300 = 3,
        S01_0400 = 4,
        S01_0500 = 5,
        S02_0000 = 6,
        S02_0100 = 7,
        S02_0200 = 8,
        S02_0300 = 9,
        S02_0400 = 10,
        S02_0450 = 11,
        S02_0500 = 12,
        S02_0600 = 13,
        S02_0700 = 14,
        S02_0800 = 15,
        S02_0900 = 16,
        S03_0000 = 17,
        S03_0100 = 18,
        S03_0200 = 19,
        S03_0300 = 20,
        S03_0350 = 21,
        S03_0370 = 22,
        S03_0400 = 23,
        S03_0500 = 24,
        S04_0000 = 25,
        S04_0050 = 26,
        S04_0100 = 27,
        S04_0200 = 28,
        S04_0300 = 29,
        S04_0400 = 30,
        S04_0500 = 31,
        S04_0600 = 32,
        S05_0000 = 33,
        S05_0010 = 34,
        S05_0050 = 35,
        S05_0080 = 36,
        S05_0100 = 37,
        S05_0200 = 38,
        S05_0300 = 39,
        S05_0400 = 40,
        S05_0500 = 41,
        S05_0600 = 42,
        Invalid = 0xFFFFFFFF,
    };
}
namespace via::attribute::RemotePropertyAttribute {
    enum class SyncMode : int64_t {
        Copy = 0,
        Kill = 1,
        Transaction = 2,
    };
}
namespace via::network::NetworkEntry {
    enum class TraceLevelType : int64_t {
        Verbose = 0,
        Info = 1,
    };
}
namespace app::ropeway::timeline::PerformanceInfo {
    enum class BindObjectState : int64_t {
        SETUP = 0,
        STANDBY = 1,
        Invalid = 2,
    };
}
namespace app::ropeway::enemy::em0000::userdata::Em0000AttackUserDataToPlayer {
    enum class ATTACK_PATTERN : int64_t {
        BITE = 0,
        NONE = 1,
    };
}
namespace via::fsm::action::Trace {
    enum class TraceType : int64_t {
        Info = 0,
        Warning = 1,
        ERROR_ = 2,
    };
}
namespace app::ropeway::EnvironmentStandbyManager {
    enum class LocationLoadPriority : int64_t {
        System = 0,
        High = 1,
        Normal = 2,
        Low = 3,
    };
}
namespace via::motion::SubExJointRemapOutput {
    enum class JointLimitFlags : int64_t {
        BasePose = 1,
    };
}
namespace app::ropeway::gui::ReNetSettingBehavior {
    enum class DecidedResult : int64_t {
        YES = 0,
        NO = 1,
        DETAIL = 2,
        MAX = 3,
        CANCEL = 4,
    };
}
namespace via::hid::camera {
    enum class PlayStationCameraAttr : int64_t {
        Ignore = 0xFFFFFFFF,
        AecAgcEnable = 0,
        AecAgcDisable = 1,
        ExposureGainMode0 = 0,
        ExposureGainMode1 = 1,
        WhiteBalanceAuto = 0,
        WhiteBalanceManual = 1,
        GammaControlOn = 0,
        GammaControlOff = 1,
    };
}
namespace via::hid::virtualKeyboard::nsw {
    enum class InputFormMode : int64_t {
        Oneline = 0,
        Multiline = 1,
        Separate = 2,
    };
}
namespace app::ropeway::timeline::action {
    enum class MotionEventKind : int64_t {
        NONE = 0,
        MESH_PARTS_ENABLE = 1,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class BreakablePartsKind : int64_t {
        BODY = 0,
        HEAD = 1,
        L_ARM = 2,
        R_ARM = 3,
        L_LEG = 4,
        R_LEG = 5,
        HAT = 6,
    };
}
namespace via::render {
    enum class BufferexSrvFlag : int64_t {
        Raw = 1,
    };
}
namespace app::ropeway::ReNetData {
    enum class NotifyType : int64_t {
        NOTIFY_00 = 0,
        NOTIFY_01 = 1,
        NOTIFY_02 = 2,
        NOTIFY_03 = 3,
        NOTIFY_04 = 4,
        NOTIFY_05 = 5,
        NOTIFY_06 = 6,
        NOTIFY_07 = 7,
        NOTIFY_08 = 8,
        NOTIFY_09 = 9,
        NOTIFY_10 = 10,
        NOTIFY_11 = 11,
        NOTIFY_12 = 12,
        NOTIFY_13 = 13,
        NOTIFY_14 = 14,
        NOTIFY_15 = 15,
        NOTIFY_16 = 16,
        NOTIFY_17 = 17,
        NOTIFY_18 = 18,
        NOTIFY_19 = 19,
    };
}
namespace app::ropeway::AssetPackagePair {
    enum class PlayerType : int64_t {
        LEON = 0,
        CLAIRE = 1,
        SHERRY = 2,
        MAX = 3,
    };
}
namespace app::ropeway::enemy::em9000::tracks::Em9000LookAtTypeTrack {
    enum class LookAtType : int64_t {
        NONE = 0,
        ALL = 1,
    };
}
namespace app::ropeway::ReNetData::OtherData {
    enum class OtherType : int64_t {
        OTHER_00 = 0,
        OTHER_01 = 1,
        OTHER_02 = 2,
        OTHER_03 = 3,
        OTHER_04 = 4,
        OTHER_05 = 5,
        OTHER_06 = 6,
        OTHER_07 = 7,
        OTHER_08 = 8,
        OTHER_09 = 9,
        OTHER_10 = 10,
        OTHER_11 = 11,
        OTHER_12 = 12,
        OTHER_13 = 13,
    };
}
namespace app::ropeway::gui::FloorMapSt52Behavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_5_212_0 = 1,
    };
}
namespace via::timeline {
    enum class BindType : int64_t {
        Children = 0,
        Scene = 1,
        Fixed = 2,
        Direct = 3,
    };
}
namespace app::ropeway::gimmick::option::GimmickOptionMotionJackSettings {
    enum class ConstType : int64_t {
        SELF = 0,
        TARGET = 1,
        NONE = 2,
    };
}
namespace app::ropeway::enemy::em3000::RoleAction {
    enum class RolePattern : int64_t {
        IDLE = 0,
        FIRST_APPEARANCE = 1,
        FALL = 2,
        DOG_EAT = 3,
        HOLD_CEILING = 4,
        FALL_TOPBOARD = 5,
        INTRUSION_TOPBOARD = 6,
    };
}
namespace via::gui {
    enum class ListInputDirection : int64_t {
        Prev = 0,
        Next = 1,
    };
}
namespace app::ropeway::enemy::em6300::Em6300ObstacleHitController {
    enum class COLLIDER_ID : int64_t {
        TerraIN_ = 0,
        Damage = 1,
        Attack = 2,
        Effect = 3,
    };
}
namespace app::ropeway::fsmv2::enemy::em7100 {
    enum class ACT_WALK_TAG : int64_t {
        Walk = 3718720945,
        Run = 3816990201,
    };
}
namespace via::fsm {
    enum class RestartType : int64_t {
        ExecuteOn = 0,
        ExecuteOff = 1,
        UseResource = 2,
        Ignore = 3,
    };
}
namespace via::network::wrangler {
    enum class EventPriority : int64_t {
        Undefined = 0,
        Normal = 1,
        Critical = 2,
        ProviderDefault = 3,
    };
}
namespace via::render::streaming_detail {
    enum class StreamingState : int64_t {
        Ready = 0,
        Request = 1,
        Complete = 2,
    };
}
namespace via::motion::IkLeg {
    enum class CenterDistance : int64_t {
        OriginalLeg = 0,
        LeachLeg = 1,
    };
}
namespace via::render::detail {
    enum class RequiredTexture : int64_t {
        IBL = 1,
        CubeMap = 2,
        LocalCubeMap = 4,
        HDR = 8,
    };
}
namespace app::ropeway::InputDefine {
    enum class VitaAssignType : int64_t {
        Vita_1 = 0,
        Vita_2 = 1,
        Vita_3 = 2,
        Vita_4 = 3,
        Vita_5 = 4,
        Vita_6 = 5,
        Vita_7 = 6,
        Vita_8 = 7,
    };
}
namespace via::effect::script::RecordOrderContainer {
    enum class Type : int64_t {
        Blood = 0,
        Scar = 1,
        Water = 2,
        Foam = 3,
    };
}
namespace via::navigation::map {
    enum class SegmentLayer : int64_t {
        Lower = 0,
        Upper = 1,
        SegmentLayerNum = 2,
    };
}
namespace via::autoplay::AutoPlayArg {
    enum class AutoPlayStatus : int64_t {
        Change = 0,
        End = 1,
    };
}
namespace via::dialog {
    enum class Result : int64_t {
        Ok = 0,
        UserCanceled = 1,
        Running = 2,
        InvalidState = 3,
        NotRunning = 4,
        UnexpectedFatal = 5,
        NotSupported = 6,
    };
}
namespace app::ropeway::enemy::em6000::fsmv2::action::Em6000FsmAction_SwitchRagdoll {
    enum class EXEC_TYPE : int64_t {
        Keep = 0,
        On = 1,
        Off = 2,
    };
}
namespace app::ropeway::level::LoadLevelManager {
    enum class LevelID : int64_t {
        INVALID = 0xFFFFFFFF,
        DEBUG_NO_LEVEL = 1,
        MAIN_STORY = 100,
        FORTH_SURVIVOR = 210,
        TOFU_SURVIVOR = 220,
        ROGUE = 300,
        L_A = 400,
        L_B = 410,
        L_C = 420,
        L_D = 430,
    };
}
namespace app::ropeway::gimmick::action::GimmickCustomJackControl::EventDataNPC {
    enum class TargetType : int64_t {
        ADA = 0,
        SHERRY = 1,
    };
}
namespace via::effect::detail {
    enum class Repeat : int64_t {
        None = 0,
        U = 1,
        V = 2,
    };
}
namespace app::ropeway::gui::FloorMapSt3CBehavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_3_604_0 = 1,
        Room_3_605_0 = 2,
        Room_3_611_0 = 3,
        Room_3_613_0c = 4,
        Room_3_614_0b = 5,
        Room_3_616_0a = 6,
        Room_3_616_0b = 7,
        Room_3_627_0 = 8,
        Room_3_630_0a = 9,
        Room_3_630_0b = 10,
        Room_3_632_0 = 11,
        Room_3_635_0a = 12,
        Room_3_635_0b = 13,
        Room_3_635_0c = 14,
        Room_3_635_0d = 15,
        Room_3_635_0e = 16,
        Room_3_638_0 = 17,
    };
}
namespace app::ropeway::network {
    enum class BrowserUrl : int64_t {
        Policy = 0,
        ReNet = 1,
        ReNetChallenge = 2,
    };
}
namespace via::effect::detail {
    enum class CullingMode : int64_t {
        Draw = 0,
        DrawAndMove = 1,
    };
}
namespace via::navigation::Navigation {
    enum class LegacySetting : int64_t {
        Trace3D = 0,
        AStar = 1,
        PortalAStar = 2,
    };
}
namespace app::ropeway::SurvivorDefine::Damage {
    enum class Type : int64_t {
        None = 0,
        StaggerSmall = 16843008,
        StaggerMedium = 16908544,
        StaggerLarge = 16974080,
        StaggerExtraLarge = 17039616,
        TossSmall = 16843264,
        TossMedium = 16908800,
        TossLarge = 16974336,
        TossExtraLarge = 17039872,
        BurstSmall = 16843520,
        BurstMedium = 16909056,
        BurstLarge = 16974592,
        BurstExtraLarge = 17040128,
        ExplosionSmall = 16843776,
        ExplosionMedium = 16909312,
        ExplosionLarge = 16974848,
        ExplosionExtraLarge = 17040384,
        Flash = 16778496,
        Shock = 16778752,
        Electric = 16779008,
        Fear = 16779264,
        Flick = 33556736,
        Shade = 33556992,
        Choke = 33557248,
    };
}
namespace via::render::RenderConfig {
    enum class FramerateType : int64_t {
        FIXING30 = 0,
        FIXING60 = 1,
        VARIABLE = 2,
    };
}
namespace via::motion::MotionCameraSkeleton {
    enum class JointHash : int64_t {
        JointNo_Transform = 0,
        JointNo_Camera = 1,
    };
}
namespace via::uvsequence {
    enum class TextureUsageType : int64_t {
        Albedo = 0,
        Normal = 1,
        Specular = 2,
        Alpha = 3,
        Num = 4,
        Unknown = 5,
    };
}
namespace app::ropeway::survivor::npc::NpcOrderDefine {
    enum class ManageCommandKind : int64_t {
        AllClear = 0,
        Invalid = 1,
    };
}
namespace app::ropeway::gamemastering::GameMaster {
    enum class CriticalRate : int64_t {
        DEFAULT = 0,
        ALL = 1,
        NOTHING = 2,
        MAX = 3,
    };
}
namespace via::render {
    enum class SysValSemantic : int64_t {
        Undefined = 0,
        Position = 1,
        Clip_Distance = 2,
        Cull_Distance = 3,
        Render_Target_Array_Index = 4,
        Viewport_Array_Index = 5,
        Vertex_Id = 6,
        Primitive_Id = 7,
        Instance_Id = 8,
        Is_Front_Face = 9,
        Sample_Index = 10,
        Final_Quad_Edge_Tessfactor = 11,
        Final_Quad_Inside_Tessfactor = 12,
        Final_Tri_Edge_Tessfactor = 13,
        Final_Tri_Inside_Tessfactor = 14,
        Final_Line_Detail_Tessfactor = 15,
        Final_Line_Density_Tessfactor = 16,
        Target = 17,
        Depth = 18,
        Coverage = 19,
        Depth_Greater_Equal = 20,
        Depth_Less_Equal = 21,
    };
}
namespace via::wwise {
    enum class TargetType : int64_t {
        OnlyThisGameObject = 0,
        IncludesOtherRelatingSounds = 1,
    };
}
namespace app::ropeway::camera {
    enum class CameraControlType : int64_t {
        INVALID = 0xFFFFFFFF,
        PLAYER = 0,
        PLAYER_SIGHT = 1,
        FIXED = 2,
        GIMMICK_FIXED = 3,
        GIMMICK_MOTION = 4,
        ACTION = 5,
        EVENT = 6,
        INVENTORY = 7,
        INTERPOLATION = 8,
        TITLE = 9,
        GAMEOVER = 10,
        RESULT = 11,
        FIGURE = 12,
        DEBUG = 13,
        DEBUG2P = 14,
    };
}
namespace app::ropeway::RogueGUIMaster {
    enum class RogueGuiState : int64_t {
        NONE = 0,
        RUCKSACK = 1,
        SHOP = 2,
        MAX = 3,
    };
}
namespace app::ropeway::OptionManager {
    enum class ControllerType : int64_t {
        TYPE_A = 0,
        TYPE_B = 1,
        TYPE_C = 2,
        TYPE_D = 3,
        TYPE_E = 4,
    };
}
namespace app::ropeway::GimmickOptionFlagSettings {
    enum class FlagStatus : int64_t {
        TRUE_ = 0,
        FALSE_ = 1,
    };
}
namespace app::ropeway::gamemastering::Item {
    enum class ID : int64_t {
        Invalid = 0,
        sm70_000 = 1,
        sm70_001 = 2,
        sm70_002 = 3,
        sm70_003 = 4,
        sm70_004 = 5,
        sm70_005 = 6,
        sm70_006 = 7,
        sm70_007 = 8,
        sm70_008 = 9,
        sm70_009 = 10,
        sm70_010 = 11,
        sm70_051 = 12,
        sm70_052 = 13,
        sm70_053 = 14,
        sm70_100 = 15,
        sm70_101 = 16,
        sm70_102 = 17,
        sm70_103 = 18,
        sm70_104 = 19,
        sm70_105 = 20,
        sm70_106 = 21,
        sm70_107 = 22,
        sm70_108 = 23,
        sm70_109 = 24,
        sm70_110 = 25,
        sm70_111 = 26,
        sm70_112 = 27,
        sm70_113 = 28,
        sm70_114 = 29,
        sm70_151 = 30,
        sm70_200 = 31,
        sm70_201 = 32,
        sm70_202 = 33,
        sm70_203 = 34,
        sm70_204 = 35,
        sm70_205 = 36,
        sm70_206 = 37,
        sm70_207 = 38,
        sm70_208 = 39,
        sm71_000 = 40,
        sm71_001 = 41,
        sm71_100 = 42,
        sm71_200 = 43,
        sm71_300 = 44,
        sm71_400 = 45,
        sm71_500 = 46,
        sm71_900 = 47,
        sm71_901 = 48,
        sm71_902 = 49,
        sm71_903 = 50,
        sm71_904 = 51,
        sm71_905 = 52,
        sm71_906 = 53,
        sm71_907 = 54,
        sm71_908 = 55,
        sm71_909 = 56,
        sm71_910 = 57,
        sm71_911 = 58,
        sm71_912 = 59,
        sm71_913 = 60,
        sm71_914 = 61,
        sm71_915 = 62,
        sm71_916 = 63,
        sm71_917 = 64,
        sm71_918 = 65,
        sm71_919 = 66,
        sm72_000 = 67,
        sm72_001 = 68,
        sm72_002 = 69,
        sm72_003 = 70,
        sm72_004 = 71,
        sm72_200 = 72,
        sm72_201 = 73,
        sm72_202 = 74,
        sm72_203 = 75,
        sm72_204 = 76,
        sm73_000 = 77,
        sm73_001 = 78,
        sm73_100 = 79,
        sm73_101 = 80,
        sm73_102 = 81,
        sm73_103 = 82,
        sm73_104 = 83,
        sm73_105 = 84,
        sm73_106 = 85,
        sm73_107 = 86,
        sm73_108 = 87,
        sm73_109 = 88,
        sm73_110 = 89,
        sm73_111 = 90,
        sm73_112 = 91,
        sm73_113 = 92,
        sm73_114 = 93,
        sm73_115 = 94,
        sm73_116 = 95,
        sm73_117 = 96,
        sm73_118 = 97,
        sm73_119 = 98,
        sm73_120 = 99,
        sm73_121 = 100,
        sm73_122 = 101,
        sm73_123 = 102,
        sm73_124 = 103,
        sm73_125 = 104,
        sm73_126 = 105,
        sm73_127 = 106,
        sm73_128 = 107,
        sm73_129 = 108,
        sm73_130 = 109,
        sm73_131 = 110,
        sm73_132 = 111,
        sm73_133 = 112,
        sm73_134 = 113,
        sm73_135 = 114,
        sm73_136 = 115,
        sm73_137 = 116,
        sm73_138 = 117,
        sm73_139 = 118,
        sm73_140 = 119,
        sm73_141 = 120,
        sm73_142 = 121,
        sm73_143 = 122,
        sm73_144 = 123,
        sm73_145 = 124,
        sm73_146 = 125,
        sm73_147 = 126,
        sm73_148 = 127,
        sm73_150 = 128,
        sm73_151 = 129,
        sm73_152 = 130,
        sm73_153 = 131,
        sm73_154 = 132,
        sm73_155 = 133,
        sm73_156 = 134,
        sm73_157 = 135,
        sm73_158 = 136,
        sm73_200 = 137,
        sm73_201 = 138,
        sm73_202 = 139,
        sm73_203 = 140,
        sm73_204 = 141,
        sm73_205 = 142,
        sm73_206 = 143,
        sm73_207 = 144,
        sm73_208 = 145,
        sm73_209 = 146,
        sm73_210 = 147,
        sm73_211 = 148,
        sm73_212 = 149,
        sm73_213 = 150,
        sm73_214 = 151,
        sm73_215 = 152,
        sm73_216 = 153,
        sm73_217 = 154,
        sm73_218 = 155,
        sm73_219 = 156,
        sm73_220 = 157,
        sm73_221 = 158,
        sm73_222 = 159,
        sm73_300 = 160,
        sm73_301 = 161,
        sm73_302 = 162,
        sm73_303 = 163,
        sm73_304 = 164,
        sm73_400 = 165,
        sm73_401 = 166,
        sm73_402 = 167,
        sm73_403 = 168,
        sm73_404 = 169,
        sm73_405 = 170,
        sm73_406 = 171,
        sm73_407 = 172,
        sm73_408 = 173,
        sm73_409 = 174,
        sm73_410 = 175,
        sm73_411 = 176,
        sm73_412 = 177,
        sm73_413 = 178,
        sm73_414 = 179,
        sm73_415 = 180,
        sm73_416 = 181,
        sm73_417 = 182,
        sm73_418 = 183,
        sm73_419 = 184,
        sm73_420 = 185,
        sm73_421 = 186,
        sm73_422 = 187,
        sm73_423 = 188,
        sm73_424 = 189,
        sm73_425 = 190,
        sm73_426 = 191,
        sm73_427 = 192,
        sm73_428 = 193,
        sm73_429 = 194,
        sm73_430 = 195,
        sm73_431 = 196,
        sm73_432 = 197,
        sm73_433 = 198,
        sm73_434 = 199,
        sm73_435 = 200,
        sm73_436 = 201,
        sm73_437 = 202,
        sm73_438 = 203,
        sm73_500 = 204,
        sm73_501 = 205,
        sm73_502 = 206,
        sm73_503 = 207,
        sm73_504 = 208,
        sm73_505 = 209,
        sm73_506 = 210,
        sm73_507 = 211,
        sm73_600 = 212,
        sm73_601 = 213,
        sm73_602 = 214,
        sm73_603 = 215,
        sm73_604 = 216,
        sm73_605 = 217,
        sm73_700 = 218,
        sm73_701 = 219,
        sm73_702 = 220,
        sm73_703 = 221,
        sm73_704 = 222,
        sm73_705 = 223,
        sm73_706 = 224,
        sm73_707 = 225,
        sm73_708 = 226,
        sm73_709 = 227,
        sm73_710 = 228,
        sm73_711 = 229,
        sm73_712 = 230,
        sm73_713 = 231,
        sm73_714 = 232,
        sm73_715 = 233,
        sm73_716 = 234,
        sm73_717 = 235,
        sm73_718 = 236,
        sm73_719 = 237,
        sm73_720 = 238,
        sm73_721 = 239,
        sm73_722 = 240,
        sm73_723 = 241,
        sm73_724 = 242,
        sm73_725 = 243,
        sm73_726 = 244,
        sm73_727 = 245,
        sm73_800 = 246,
        sm73_801 = 247,
        sm73_802 = 248,
        sm73_803 = 249,
        sm74_000 = 250,
        sm74_100 = 251,
        sm74_101 = 252,
        sm74_102 = 253,
        sm74_103 = 254,
        sm74_104 = 255,
        sm74_105 = 256,
        sm74_106 = 257,
        sm74_107 = 258,
        sm74_108 = 259,
        sm74_109 = 260,
        sm74_110 = 261,
        sm74_200 = 262,
        sm74_201 = 263,
        sm75_000 = 264,
        sm75_001 = 265,
        sm75_002 = 266,
        sm75_003 = 267,
        sm75_004 = 268,
        sm75_005 = 269,
        sm75_006 = 270,
        sm75_007 = 271,
        sm75_008 = 272,
        sm75_009 = 273,
        sm75_010 = 274,
        sm75_011 = 275,
        sm75_012 = 276,
        sm75_013 = 277,
        sm75_014 = 278,
        sm75_015 = 279,
        sm75_016 = 280,
        sm75_017 = 281,
        sm75_018 = 282,
        sm75_019 = 283,
        sm75_020 = 284,
        sm75_021 = 285,
        sm75_022 = 286,
        sm75_023 = 287,
        sm76_000 = 288,
        sm77_000 = 289,
        sm77_001 = 290,
        sm77_002 = 291,
        sm77_003 = 292,
        sm77_004 = 293,
        sm77_005 = 294,
        sm77_006 = 295,
        sm77_007 = 296,
        MAX = 297,
    };
}
namespace app::ropeway::CollidersExtention::FindInfo {
    enum class LayerType : int64_t {
        TerraIN_ = 0,
        Effect = 1,
        Character = 2,
        Marker = 3,
        Notice = 4,
        Press = 5,
    };
}
namespace app::ropeway::enemy::em6000::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_MOVE = 1,
        ACT_PIVOTTURN = 2,
        ACT_THREAT = 3,
        ACT_HIDE = 4,
        ACT_FLOAT_IDLE = 5,
        ACT_THREAT_IDLE = 6,
        ACT_FILL_IN_IDLE = 7,
        ACT_DRINK_SEWAGE = 8,
        ACT_DRINK_SEWAGE_END = 9,
        ACT_LOOK_AROUND = 10,
        ATK_PUNCH = 2000,
        ATK_DASH_PUNCH = 2001,
        ATK_BACK_SLAP = 2002,
        ATK_BACK_SLASH = 2003,
        ATK_NEAR_GRAPPLE = 2004,
        ATK_FAR_GRAPPLE = 2005,
        ATK_HOLD = 2006,
        ATK_RIGHT_HOOK = 2007,
        ATK_VOMIT_CHILDREN = 2008,
        ATK_UPPER = 2009,
        DMG_STUN = 3000,
        DMG_STUN_EYE = 3001,
        DMG_KNOCKBACK = 3002,
        DMG_BROKEN_PUS = 3003,
        DMG_FLASH = 3004,
        DMG_BURN = 3005,
        DMG_ELECTRIC = 3006,
        DMG_ELECTRIC_FINISH = 3007,
        DMG_SPT_GRENADE_BOMB = 3008,
        DMG_SPT_FLASH_GRENADE_BOMB = 3009,
        DMG_OVIDUCT = 3010,
        SPT_GRENADE = 4000,
        SPT_KNIFE = 4001,
        DIE_DEAD = 5000,
        DIE_RIGID = 5001,
        SET_IDLE = 6000,
        SET_FLOATATION = 6001,
        SET_DEAD = 6002,
        SET_CAVE_APPEAR = 6003,
        SET_CAVE_APPEAR2 = 6004,
        SET_CEILING_APPEAR = 6005,
        SET_DIVE = 6006,
        SET_SWIM_GRAPPLE = 6007,
        SET_DRINK_SEWAGE = 6008,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace via::motion::detail::ChainNodeData {
    enum class AngleMode : int64_t {
        Free = 0,
        LimitCone = 1,
        LimitHinge = 2,
        LimitConeBox = 3,
        LimitOval = 4,
        LimitElliptic = 5,
    };
}
namespace app::ropeway::gimmick::option::SurvivorJackActionControlSettings::Param {
    enum class TargetKind : int64_t {
        Player = 0,
        Npc = 1,
    };
}
namespace app::ropeway::enemy::em6000 {
    enum class PusPartsID : int64_t {
        ID_01 = 0,
        ID_02 = 1,
        ID_03 = 2,
        ID_04 = 3,
        ID_05 = 4,
        ID_06 = 5,
        ID_07 = 6,
        ID_08 = 7,
        ID_09 = 8,
        ID_10 = 9,
        ID_11 = 10,
        ID_12 = 11,
        ID_13 = 12,
        NUM = 13,
        Invalid = 0xFFFFFFFF,
    };
}
namespace app::ropeway::OptionManager {
    enum class ShadowQuality : int64_t {
        LOWEST = 0,
        LOW = 1,
        STANDARD = 2,
        HIGH = 3,
        HIGHEST = 4,
    };
}
namespace via::network::wrangler {
    enum class BlobType : int64_t {
        Unknown = 0,
        String = 1,
        GUID = 2,
        Int32 = 3,
        UInt32 = 4,
        Int64 = 5,
        UInt64 = 6,
        Float = 7,
        Double = 8,
        Boolean = 9,
    };
}
namespace via::render {
    enum class DisplaySyncExtension : int64_t {
        None = 0,
        FreeSync2 = 1,
    };
}
namespace via::hid {
    enum class VrTrackerDeviceType : int64_t {
        HMD = 0,
        Dualshock4 = 1,
    };
}
namespace via::render {
    enum class StreamingTextureRequestStrategy : int64_t {
        Default = 0,
        InsideBounding = 1,
        DistanceFromBounding = 2,
    };
}
namespace System::Reflection {
    enum class MethodAttributes : int64_t {
        MemberAccessMask = 7,
        PrivateScope = 0,
        Private = 1,
        FamANDAssem = 2,
        Assembly = 3,
        Family = 4,
        FamORAssem = 5,
        Public = 6,
        Static = 16,
        Final = 32,
        Virtual = 64,
        HideBySig = 128,
        CheckAccessOnOverride = 512,
        VtableLayoutMask = 256,
        ReuseSlot = 0,
        NewSlot = 256,
        Abstract = 1024,
        SpecialName = 2048,
        PinvokeImpl = 8192,
        UnmanagedExport = 8,
        RTSpecialName = 4096,
        HasSecurity = 16384,
        RequireSecObject = 32768,
        ReservedMask = 53248,
    };
}
namespace via::render {
    enum class AsyncExecuteFlag : int64_t {
        Disable = 0,
        CommandPriorityWait = 1,
        NoWait = 2,
        CurrentFrameExecute = 3,
        CurrentFrameExecuteNoWait = 4,
    };
}
namespace app::ropeway::gui::DialogBehavior {
    enum class Mode : int64_t {
        NO_YES__SELECTED_NO = 0,
        NO_YES__SELECTED_YES = 1,
        YES_NO__SELECTED_YES = 2,
        YES_NO__SELECTED_NO = 3,
        YES = 4,
        INVISIBLE_BUTTON = 5,
        DISABLE_INPUT = 6,
        EVENT_PAUSE = 7,
        YES_NO__SELECTED_YES__NO_CANCEL = 8,
        YES_NO__SELECTED_NO__NO_CANCEL = 9,
    };
}
namespace via::motion::IkSpine {
    enum class RAY_TYPE : int64_t {
        DEFAULT = 0,
        JOINT = 1,
    };
}
namespace app::ropeway::TerrainAnalyzer {
    enum class AnalyzeMode : int64_t {
        DIRECTION = 0,
        MOVE_DIRECTION = 1,
        SPECIFICATION = 2,
    };
}
namespace via::motion::JointExMultiRemapValue::ConeInputData {
    enum class AttrFlags : int64_t {
        None = 0,
        BasePose = 1,
    };
}
namespace app::ropeway::enemy::EmCommonContext {
    enum class InstanceStatus : int64_t {
        NOT_AVAILABLE = 0,
        INSTANCING = 1,
        AVAILABLE = 2,
        DESTROYING = 3,
    };
}
namespace via::gui {
    enum class SoftParticleDistType : int64_t {
        System = 0,
        Component = 1,
        Disable = 2,
    };
}
namespace app::ropeway::gui::FloorMapSt3BBehavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_1_623_0b = 1,
        Room_3_600_0a = 2,
        Room_3_600_0b = 3,
        Room_3_602_0a = 4,
        Room_3_602_0b = 5,
        Room_3_603_0 = 6,
        Room_3_604_0 = 7,
        Room_3_611_0a = 8,
        Room_3_612_0 = 9,
        Room_3_613_0a = 10,
        Room_3_613_0b = 11,
        Room_3_614_0a = 12,
        Room_3_615_0 = 13,
        Room_3_630_0c = 14,
        Room_3_630_0d = 15,
        Room_3_640_0a = 16,
        Room_3_640_0b = 17,
        Room_3_641_0a = 18,
        Room_3_641_0b = 19,
        Room_3_642_0 = 20,
    };
}
namespace via::nnfc {
    enum class ERRORVIEWER_CODE : int64_t {
        ERRORVIEWER_WIRELESS_OFF = 0,
        ERRORVIEWER_NFP_NOT_SUPPORT = 1,
        ERRORVIEWER_NFP_INVALID_FORMAT_VER = 2,
    };
}
namespace via::motion::JointExMultiRemapValue::OutputData::JointDriverData {
    enum class AttrFlags : int64_t {
        None = 0,
        BasePose = 1,
        MidPoint = 2,
    };
}
namespace via::gui {
    enum class HitAreaShape : int64_t {
        Triangle = 0,
        Rect = 1,
        Hexagon = 2,
        Octagon = 3,
    };
}
namespace app::ropeway::MinimapLocater {
    enum class PartsNo : int64_t {
        invalid = 0,
        Parts01 = 1,
        Parts02 = 2,
        Parts03 = 3,
        Parts04 = 4,
        Parts05 = 5,
        Parts06 = 6,
        Parts07 = 7,
        Parts08 = 8,
        Parts09 = 9,
        Parts10 = 10,
    };
}
namespace app::ropeway::EquipmentDefine {
    enum class SubWeaponType : int64_t {
        Invalid = 0xFFFFFFFF,
        None = 0,
        Knife = 1,
        InfinityKnife = 2,
        Grenade = 3,
        FlashGranede = 4,
        Barre = 5,
        Bottle = 6,
    };
}
namespace via::behaviortree::SelectorCallerChildNodeEnd {
    enum class CheckType : int64_t {
        AnyNode = 0,
        AllNode = 1,
    };
}
namespace via::memory {
    enum class AllocatorType : int64_t {
        Boot = 0,
        Default = 1,
        Permanent = 2,
        Resource = 3,
        Develop = 4,
        Temp = 5,
        VRAM = 6,
        Sound = 7,
        Max = 8,
    };
}
namespace via::render::RenderConfig {
    enum class RenderingMethod : int64_t {
        Normal = 0,
        Checkerboard = 1,
        Interlaced = 2,
    };
}
namespace via::effect::script::MathEx {
    enum class RotationOrder : int64_t {
        XYZ = 0,
        XZY = 1,
        YXZ = 2,
        YZX = 3,
        ZXY = 4,
        ZYX = 5,
    };
}
namespace via::render::AmbientOcclusionGeometryElement {
    enum class GeometryType : int64_t {
        Sphere = 0,
        Capsule = 1,
        None = 2,
    };
}
namespace via {
    enum class memory_order : int64_t {
        relaxed = 0,
        consume = 1,
        acquire = 2,
        release = 3,
        acq_rel = 4,
        seq_cst = 5,
    };
}
namespace app::ropeway::gimmick::option::SurvivorControlSettings {
    enum class Status : int64_t {
        Start = 0,
        Exec = 1,
        PreEnd = 2,
        Wait = 3,
    };
}
namespace via::effect::detail {
    enum class Shape2DType : int64_t {
        Square = 0,
        Circle = 1,
    };
}
namespace via::motion::detail::ChainLinkData {
    enum class ConnectionFlags : int64_t {
        Neighbour = 1,
        Upper = 2,
        Bottom = 4,
    };
}
namespace app::ropeway::gamemastering::Location {
    enum class Type : int64_t {
        MainContents = 0,
        RogueContents = 1,
    };
}
namespace via::hid::hmd::Morpheus {
    enum class VrModeStatusCheckLevel : int64_t {
        Nothing = 0,
        Warning = 1,
        ERROR_ = 2,
        Assertion = 3,
    };
}
namespace app::ropeway::ReNetData::GameOverData {
    enum class CauseType : int64_t {
        CAUSE_00 = 0,
        CAUSE_01 = 1,
        CAUSE_02 = 2,
        CAUSE_03 = 3,
        CAUSE_04 = 4,
        CAUSE_05 = 5,
        CAUSE_06 = 6,
        CAUSE_07 = 7,
        CAUSE_08 = 8,
        CAUSE_09 = 9,
        CAUSE_10 = 10,
        CAUSE_11 = 11,
        CAUSE_12 = 12,
        CAUSE_13 = 13,
        CAUSE_14 = 14,
        CAUSE_15 = 15,
        CAUSE_16 = 16,
        UNKNOWN = 17,
    };
}
namespace app::ropeway::fsmv2::GmkAlligatorRun {
    enum class ActionType : int64_t {
        FINISH = 0,
    };
}
namespace app::ropeway::enemy::em3000::IdleVariation {
    enum class Pattern : int64_t {
        IDLE_00 = 0,
        IDLE_01 = 1,
        IDLE_02 = 2,
        IDLE_03 = 3,
    };
}
namespace via::AnimationCurveData {
    enum class Wrap : int64_t {
        Once = 0,
        Loop = 1,
        Loop_Always = 2,
    };
}
namespace via::effect::script::EffectCullingZoneGroup {
    enum class Status : int64_t {
        None = 0,
        CullingOnTrigger = 1,
        CullingOffTrigger = 2,
        CullingOff = 3,
    };
}
namespace app::ropeway::SceneActivateManager::RequestInfo {
    enum class Status : int64_t {
        Initial = 0,
        ReadyForActive = 1,
        Active = 2,
        ReadyForDeactive = 3,
        Deactive = 4,
    };
}
namespace app::ropeway::enemy::tracks::Em6200ColliderTrack {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Sensor_Enemy = 4,
        Attack_Hook_R = 5,
        Attack_Hook_L = 6,
        Attack_Punch_R = 7,
        Attack_Punch_L = 8,
        Attack_Hammer = 9,
        Attack_TurnBlow_L = 10,
        Attack_TurnBlow_R = 11,
        Attack_Flash = 12,
        Attack_Fall = 22,
        Tangle_Push_R = 13,
        Tangle_Push_L = 14,
        Tangle_Kick_R = 15,
        Tangle_Kick_L = 16,
        Tangle_Punch = 17,
        Tangle_Fall = 23,
        Glapple_Hold = 18,
        Glapple_Throw = 19,
    };
}
namespace via::hid {
    enum class NpadStyle : int64_t {
        Handheld = 1,
        FullKey = 2,
        JoyDual = 4,
        JoyLeft = 8,
        JoyRight = 16,
    };
}
namespace app::ropeway::gamemastering::RogueUIMapManager {
    enum class LostIconType : int64_t {
        INVALID = 0xFFFFFFFF,
        RUCKSACK = 0,
        SHOP = 1,
    };
}
namespace app::ropeway::enemy::common::fsmv2::condition::EmCommonCondition_StairsDirection {
    enum class StairsDirection : int64_t {
        TOP = 0,
        BOTTOM = 1,
    };
}
namespace via::render {
    enum class DepthWriteMask : int64_t {
        Zero = 0,
        All = 1,
        Num = 2,
    };
}
namespace via::render::detail {
    enum class CullingDataType : int64_t {
        None = 0,
        OBB = 1,
        Sphere = 2,
        AABB = 3,
        Distance = 4,
        Cone = 5,
    };
}
namespace System::Reflection {
    enum class BindingFlags : int64_t {
        Default = 0,
        IgnoreCase = 1,
        DeclaredOnly = 2,
        Instance = 4,
        Static = 8,
        Public = 16,
        NonPublic = 32,
        FlattenHierarchy = 64,
        InvokeMethod = 256,
        CreateInstance = 512,
        GetField = 1024,
        SetField = 2048,
        GetProperty = 4096,
        SetProperty = 8192,
        PutDispProperty = 16384,
        PutRefDispProperty = 32768,
        ExactBinding = 65536,
        SuppressChangeType = 131072,
        OptionalParamBinding = 262144,
        IgnoreReturn = 16777216,
    };
}
namespace app::ropeway::gui::FloorMapSt5ABehavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_5_101_0 = 1,
        Room_5_102_0 = 2,
        Room_5_103_0 = 3,
        Room_5_104_0a = 4,
        Room_5_104_0b = 5,
        Room_5_105_0 = 6,
        Room_5_106_0 = 7,
        Room_5_111_0a = 8,
        Room_5_111_0b = 9,
        Room_5_111_0c = 10,
        Room_5_111_0d = 11,
        Room_5_121_0 = 12,
        Room_5_122_0 = 13,
        Room_5_122_1 = 14,
        Room_5_122_2 = 15,
        Room_5_123_0 = 16,
        Room_5_124_0 = 17,
        Room_5_125_0 = 18,
        Room_5_131_0 = 19,
        Room_5_132_0 = 20,
        Room_5_133_0 = 21,
        Room_5_134_0 = 22,
    };
}
namespace app::ropeway::enemy::em6200::MotionPattern {
    enum class DoorOverTurn : int64_t {
        DoorL_L = 0,
        DoorL_R = 1,
        DoorR_L = 2,
        DoorR_R = 3,
        DoubleDoorL_L = 4,
        DoubleDoorL_R = 5,
        DoubleDoorR_L = 6,
        DoubleDoorR_R = 7,
    };
}
namespace app::ropeway::gimmick::action::InteractManager {
    enum class FilteringPriority : int64_t {
        FilteringAllLock = 0,
        FilteringAllNoInGame = 1,
        FilteringAllGUIInventory = 2,
        FilteringAllGUIPause = 3,
        FilteringAllTimeline = 4,
        FilteringAllPlDying = 5,
        FilteringPlAttacked = 100,
        FilteringGimmickEvent = 101,
        FilteringPlJacked = 102,
        FilteringPlHoldWep = 200,
        FilteringGimmickEx = 300,
        FilteringPlJackedLowPriority = 301,
        FilteringNone = 9999,
    };
}
namespace app::ropeway::OptionManager {
    enum class CameraReverseType : int64_t {
        NORMAL = 0,
        UD_REVERSE = 1,
        LR_REVERSE = 2,
        UDLR_REVERSE = 3,
    };
}
namespace via::motion::IkJacobian::Link {
    enum class AxisType : int64_t {
        X = 1,
        Y = 2,
        Z = 4,
    };
}
namespace via::storage::saveService {
    enum class SaveDataVersion : int64_t {
        SaveDataVersion_None = 0,
        SaveDataVersion_1_ClassInheritance = 1,
        SaveDataVersion_2_IsNotFindSystemType = 2,
        SaveDataVersion_Max = 3,
    };
}
namespace via::motion::GpuMotion {
    enum class WrapMode : int64_t {
        Once = 0,
        Loop = 1,
    };
}
namespace via::motion {
    enum class ChainVGroundType : int64_t {
        None = 0,
        Root = 1,
        Target = 2,
    };
}
namespace app::ropeway::timeline::TimelineFadeController {
    enum class UpdateMode : int64_t {
        Default = 0,
        Timeline = 1,
    };
}
namespace app::ropeway::EnvironmentStandbyController {
    enum class Target : int64_t {
        None = 0,
        Reservation = 1,
        Standby = 2,
        Active = 3,
    };
}
namespace app::ropeway::enemy::em3000::Em3000FootAdjust {
    enum class RotateFixedType : int64_t {
        NONE = 0,
        AXIS_Y = 1,
        ALL = 2,
    };
}
namespace via::hid {
    enum class GamePadMotor : int64_t {
        Motor0 = 0,
        Motor1 = 1,
        Motor2 = 2,
        Motor3 = 3,
        Max = 4,
        NULL_ = 5,
        All = 6,
        LowFrequencyMotor = 128,
        HighFrequencyMotor = 129,
        LAnalogTriggerMotor = 130,
        RAnalogTriggerMotor = 131,
    };
}
namespace app::ropeway::gui::SelectBrightnessBehavior {
    enum class EndType : int64_t {
        SELECTED = 0,
        CANCELED = 1,
    };
}
namespace app::ropeway::enemy::em6000::userdata::Em6000ParamUserData::AttackParam {
    enum class ATTRIBUTE : int64_t {
        IgnoreCameTarget = 0,
        IgnoreGoAwayTarget = 1,
        EyeHiddenDisable = 2,
        GroundDisable = 3,
        ContinuousDisable = 4,
        LockOnOnly = 5,
        DownOnly = 6,
    };
}
namespace via::fsm {
    enum class ExpressionReferenceType : int64_t {
        LocalUserData = 0,
        GlobalUserData = 1,
        Direct = 2,
    };
}
namespace app::ropeway::OptionManager {
    enum class SoundMode : int64_t {
        NORMAL = 0,
        CHANGED_0 = 1,
    };
}
namespace via::hid {
    enum class NpadConnectionAttribute : int64_t {
        IsConnected = 1,
        IsWired = 2,
        IsLeftConnected = 4,
        IsLeftWired = 8,
        IsRightConnected = 16,
        IsRightWired = 32,
        IsDebugPadConnected = 2147483648,
    };
}
namespace app::ropeway::fsmv2::GmkEnduringBlockPrefabRegister {
    enum class State : int64_t {
        REGISTER = 0,
        UNREGISTER = 1,
    };
}
namespace via::effect::script::StampController {
    enum class SaveStateEnum : int64_t {
        None = 0,
        Preparing = 1,
        Ready = 2,
    };
}
namespace via::render::layer {
    enum class OutlineRenderSegment : int64_t {
        DrawMask = 0,
        DrawOutline = 5,
        Copy = 7,
        Lowest = 63,
    };
}
namespace app::ropeway::network::service::StorageServiceController {
    enum class Phase : int64_t {
        None = 0,
        Setup = 1,
        GetInfo = 2,
        GetInfoWait = 3,
        OpenWait = 4,
        Read = 5,
        ReadWait = 6,
        Finalize = 7,
    };
}
namespace via::os::dma {
    enum class DMA_RESULT : int64_t {
        DMA_SUCCESS = 0,
        DMA_INVARG = 0,
        DMA_FAIL = 0,
    };
}
namespace app::ropeway::enemy::em3000::fsmv2::transitionevent::ForceRequestActionEvent {
    enum class StandupPattern : int64_t {
        FaceUpF = 0,
        FaceUpB = 1,
        FaceDown = 2,
        Auto = 3,
    };
}
namespace app::ropeway::enemy::em6300::fsmv2::action::Em6300FsmAction_CheckCombo {
    enum class CHECK_BIT : int64_t {
        NAIL_2 = 0,
        POWER_DUNK = 1,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::EnemyActionCameraPlayRequestRoot {
    enum class BankIDSelectType : int64_t {
        SET_BANKID = 0,
        LATEST_BANKID = 1,
        RANDOM = 2,
    };
}
namespace app::ropeway::Em4000Define {
    enum class RequestSetID : int64_t {
        PRESS = 0,
        DAMAGE = 1,
        SENSOR_TOUCH = 2,
        ATTACK_BITE = 3,
        ATTACK_HOLD = 4,
    };
}
namespace via::hid {
    enum class VibrationPriority : int64_t {
        Lowest = 0,
        Low = 2,
        Middle = 3,
        High = 4,
        Highest = 6,
    };
}
namespace via::motion::MotionData {
    enum class AttrFlags : int64_t {
        GpuMot = 1,
    };
}
namespace via::motion::CppSampleAppendData {
    enum class Test : int64_t {
        A = 0,
        B = 1,
        C = 2,
    };
}
namespace via::hid::virtualKeyboard::nsw {
    enum class InvalidCharFlag : int64_t {
        None = 0,
        Space = 2,
        AtMark = 4,
        Percent = 8,
        Slash = 16,
        BackSlash = 32,
        Numeric = 64,
        OutsideOfDownloadCode = 128,
        OutsideOfMiiNickName = 256,
    };
}
namespace app::ropeway::TerrainAnalyzer {
    enum class SideType : int64_t {
        CENTER = 0,
        LEFT = 1,
        RIGHT = 2,
    };
}
namespace via::effect::gpgpu::detail {
    enum class GpuItemType : int64_t {
        Billboard = 0,
        RibbonFollow = 1,
        Life = 2,
        UVSequence = 3,
        Velocity = 4,
        RotateAnim = 5,
        ScaleAnim = 6,
        DepthOperator = 7,
        WindInfluence = 8,
        ShapeOperator = 9,
        MaxType = 10,
    };
}
namespace via::motion::IkJacobian {
    enum class InverseMethodType : int64_t {
        LU = 0,
        Cholesky = 1,
        DebugCompare = 2,
    };
}
namespace via::network::session {
    enum class SearchByFilterOption : int64_t {
        None = 0,
        GetPerformance = 2,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::EmCommonMFsmAction_GameRankMotionSpeedControl {
    enum class GetRateTiming : int64_t {
        START = 0,
        ALWAYS = 1,
    };
}
namespace app::ropeway::enemy::em7000::Em7000Define {
    enum class AttackRangeType : int64_t {
        Range_NearAttack = 0,
        Range_NormalAttack = 1,
        Range_FarAttack = 2,
        Range_DeathAttack = 3,
        Range_NoAttackMode = 4,
    };
}
namespace app::ropeway::HIDDefine {
    enum class HIDCommandType : int64_t {
        None = 0,
        Hold = 1,
        HoldSub = 2,
        Attack = 3,
        Reload = 4,
        ChangeAmmo = 5,
        SwitchWeaponLeft = 6,
        SwitchWeaponRight = 7,
        SwitchWeaponUp = 8,
        SwitchWeaponDown = 9,
        MoveForward = 10,
        MoveBackward = 11,
        MoveLeft = 12,
        MoveRight = 13,
        DashToggle = 14,
        QuickTurn = 15,
        ResetCamera = 16,
        Interact = 17,
        Pause = 18,
        Map = 19,
        Inventory = 20,
        Confirm = 21,
        Cancel = 22,
        CursorUp = 23,
        CursorDown = 24,
        CursorLeft = 25,
        CursorRight = 26,
        SwitchTabLeft = 27,
        SwitchTabRight = 28,
        AreaLeft = 29,
        AreaRight = 30,
        MoveItem = 31,
        SortInventory = 32,
        ItemSetSwitchScreen = 33,
        ItemSetAdd = 34,
        ItemSetUse = 35,
        MapUp = 36,
        MapDown = 37,
        PageUp = 38,
        PageDown = 39,
        ScaleUp = 40,
        ScaleDown = 41,
        ScaleDefault = 42,
        DispDescription = 43,
    };
}
namespace app::ropeway::enemy::em4000::behaviortree::condition::Em4000BtCondition_CaseRange {
    enum class RANGE_TYPE : int64_t {
        RANGE_1 = 0,
        RANGE_2 = 1,
        RANGE_3 = 2,
        RANGE_4 = 3,
    };
}
namespace app::ropeway::gimmick::option::EnemyControlSettings::EnemyControlParam {
    enum class SetAreaControlType : int64_t {
        SameAreaWithPL = 0,
        UpdateOnHere = 1,
    };
}
namespace System {
    enum class DateTimeKind : int64_t {
        Unspecified = 0,
        Utc = 1,
        Local = 2,
    };
}
namespace via::attribute::RemoteField {
    enum class SyncMode : int64_t {
        Copy = 0,
        Kill = 1,
        Transaction = 2,
    };
}
namespace via::render {
    enum class LodResourceType : int64_t {
        Global = 0,
        Local = 1,
        Unknown = 2,
    };
}
namespace via {
    enum class CurveType : int64_t {
        FlatHermite = 0,
        Linear = 1,
        Constant = 2,
        Hermite = 3,
        Broken = 4,
        DetailsHermite = 5,
        DetailsBroken = 6,
    };
}
namespace app::ropeway::gui::NewInventoryBehavior {
    enum class TabMode : int64_t {
        Map = 0,
        Item = 1,
        File = 2,
    };
}
namespace via::render::layer {
    enum class ShadowCastStrategy : int64_t {
        ExplicitStaticMesh = 1,
        ImplicitStaticMesh = 2,
        DynamicMesh = 4,
        Default = 7,
        UsingSST = 6,
        CreateSST = 1,
    };
}
namespace app::ropeway::NoticePoint {
    enum class ResultKind : int64_t {
        Unknown = 0,
        Shielded = 1,
        Through = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickCustomJackControl {
    enum class EventState : int64_t {
        BEFORE = 0,
        BEFORE2 = 1,
        GO = 2,
        AFTER = 3,
        END = 4,
    };
}
namespace app::ropeway::gui::FloorMapSt42Behavior {
    enum class BgObjectId : int64_t {
        Barricade_RPD2_00 = 0,
        Barricade_RPD2_01 = 1,
        Barricade_RPD2_02 = 2,
        Barricade_RPD2_03 = 3,
        Barricade_RPD2_04 = 4,
    };
}
namespace app::ropeway {
    enum class SoundAppealPriority : int64_t {
        HIGHEST = 0,
        GUNSHOT = 1,
        FOOTSTEP = 2,
        GUNHIT = 3,
        LOWEST = 4,
    };
}
namespace app::ropeway::enemy::userdata::Em4000DamageUserData {
    enum class RegionType : int64_t {
        Head = 0,
        Body = 1,
        Leg = 2,
    };
}
namespace via::navigation {
    enum class FilterTarget : int64_t {
        Default = 0,
        Groups = 1,
    };
}
namespace app::ropeway::enemy::em4400::Em4400ThinkChild {
    enum class ACTION_ID : int64_t {
        Wait = 0,
        Turn = 1,
        RandomMove = 2,
        ReturnToBase = 3,
        Escape = 4,
        Hide = 5,
        TrayWait = 6,
        TrayMove = 7,
        TrayEscape = 8,
        TrayFall = 9,
    };
}
namespace app::ropeway::enemy::em7400 {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Damage = 0,
        Attack_Kill = 1,
        Attack_Appear = 2,
        Attack_Appear2 = 3,
        Attack1 = 4,
        Attack2 = 5,
    };
}
namespace app::ropeway::gamemastering::WakeUpFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        OPEN_CATALOG = 1,
        WAIT_OPENING_CATALOG = 2,
        OPEN_OBJECTPOOL = 3,
        WAIT_OPENING_OBJECTPOOL = 4,
        LOAD_GUI_TITLE = 5,
        WAIT_GUI_TITLE = 6,
        AWAKE_WAKEUP = 7,
        START_WAKEUP = 8,
        UPDATE_WAKEUP = 9,
        WAKEUP_LOAD_FONT = 10,
        WAKEUP_WAIT_FONT = 11,
        CHECK_USER_DEVICE_PAIRING = 12,
        WAIT_USER_DEVICE_PAIRING = 13,
        CHECK_ROGUE_SYSTEM_SAVEDATA = 14,
        LOAD_ROGUE_SYSTEM_SAVEDATA = 15,
        WAIT_ROGUE_SYSTEM_SAVEDATA = 16,
        CHECK_SYSTEM_SAVEDATA = 17,
        FIRST_BOOT = 18,
        FIRST_WAIT_FONT = 19,
        NORMAL_BOOT = 20,
        WAIT_SYSTEM_SAVEDATA = 21,
        CHECK_EXTRA_CONTENTS = 22,
        CHECK_LOST_EXTRA_CONTENTS = 23,
        WARNING_AUTO_SAVE = 24,
        BRANCH_FIRST_WAKE_UP = 25,
        SET_LANGUAGE = 26,
        SET_LANGUAGE_WAIT_FONT = 27,
        SET_1ST_SELECT_HDR_SDR = 28,
        SET_1ST_BRANCH_SETTING = 29,
        SET_1ST_BRANCH_WAIT = 30,
        SET_1ST_BRANCH_FAILED = 31,
        SET_1ST_BRANCH_HDR_SDR = 32,
        SET_1ST_BRANCH_HDR_SDR_SHOW_DIALOG_FOR_PC = 33,
        SET_BRIGHTNESS_MAX = 34,
        SET_BRIGHTNESS_MIN_ = 35,
        SET_BRIGHTNESS = 36,
        SET_HDR_BRIGHTNESS_MAX = 37,
        SET_HDR_BRIGHTNESS = 38,
        SET_SCREEN_SIZE = 39,
        SET_RE_NET = 40,
        CHECK_PRIVACY_POLICY = 41,
        CHECK_DLC_ANNOUNCEMENT = 42,
        START_OR_OPTION = 43,
        SET_COSTUME = 44,
        SET_OPTION = 45,
        SET_CAMERA = 46,
        SET_DIFFICULTY = 47,
        SETUP_FIRST_GAME = 48,
        SET_2ND_SELECT_HDR_SDR = 49,
        SET_2ND_BRANCH_SETTING = 50,
        SET_2ND_BRANCH_WAIT = 51,
        SET_2ND_BRANCH_FAILED = 52,
        SET_2ND_BRANCH_HDR_SDR = 53,
        SET_2ND_BRANCH_HDR_SDR_SHOW_DIALOG_FOR_PC = 54,
        SET_2ND_BRIGHTNESS_MAX = 55,
        SET_2ND_BRIGHTNESS_MIN_ = 56,
        SET_2ND_BRIGHTNESS = 57,
        SET_2ND_HDR_BRIGHTNESS_MAX = 58,
        SET_2ND_HDR_BRIGHTNESS = 59,
        SET_FORCE_DX11_CLASH = 60,
        SET_FORCE_DX11_CAPABILITY = 61,
        FINALIZE = 62,
        WAIT_GUI_ANIMATION = 100,
    };
}
namespace via::hid::virtualKeyboard::nsw {
    enum class Preset : int64_t {
        Default = 0,
        Password = 1,
        UserName = 2,
        DownloadCode = 3,
    };
}
namespace app::ropeway::gui::RogueKeyFlagsBehavior {
    enum class SubState : int64_t {
        INVALID = 0xFFFFFFFF,
        NONE = 0,
        START = 1,
        WAIT_END_ANIM = 2,
        WAIT_INPUT = 3,
        END = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickCircuitPuzzle {
    enum class InputResult : int64_t {
        SUCCESS = 0,
        LOSE = 1,
        STOP = 2,
    };
}
namespace via::clr {
    enum class EnumU4 : int64_t {
        Dummy = 0,
    };
}
namespace System::Runtime::InteropServices {
    enum class CharSet : int64_t {
        None = 1,
        Ansi = 2,
        Unicode = 3,
        Auto = 4,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class Sound : int64_t {
        Setting = 1001,
        Update = 1002,
    };
}
namespace app::ropeway::gimmick::action::GimmickBody {
    enum class GimmickKind : int64_t {
        UNDEFINED = 0,
        DOOR = 1,
        WINDOW = 2,
        LADDER = 3,
        BOX = 4,
        TERRAINSTEP = 5,
        RAISESHELF = 6,
        CARRYSHERRY = 7,
        ALLIGATORRUN = 8,
        SLOPE = 9,
        BOX_FOR_SHERRY = 10,
        JUMP_DOWN = 11,
    };
}
namespace System {
    enum class NumberStyles : int64_t {
        None = 0,
        AllowLeadingWhite = 1,
        AllowTrailingWhite = 2,
        AllowLeadingSign = 4,
        AllowTrailingSign = 8,
        AllowParentheses = 16,
        AllowDecimalPoint = 32,
        AllowThousands = 64,
        AllowExponent = 128,
        AllowCurrencySymbol = 256,
        AllowHexSpecifier = 512,
        Integer = 7,
        HexNumber = 515,
        Number = 111,
        Float = 167,
        Currency = 383,
        Any = 511,
    };
}
namespace app::ropeway::weapon::shell::ShellCartridgeController {
    enum class WeaponTypes : int64_t {
        Revolver = 0,
        Automatic = 1,
    };
}
namespace app::ropeway::enemy::em6000 {
    enum class ColliderStateTye : int64_t {
        Normal = 0,
        Floating = 1,
        Ambush = 2,
    };
}
namespace app::ropeway::weapon::shell::ShellManager {
    enum class WeaponShellTypeKind : int64_t {
        Bullet_Default = 0,
        Bullet_Grenade_Acid = 1,
        Bullet_Grenade_Fire = 2,
        Bullet_Rocket_Normal = 3,
        Bullet_Rocket_Infinity = 4,
        Bullet_Spark = 5,
        Bullet_Pus = 6,
        Bomb_Explosion = 7,
        Bomb_Blow = 8,
        Bomb_PlReactionExplosion = 9,
        Bomb_PlReactionBlow = 10,
        Spark_Small = 11,
        Spark_Large = 12,
        Throw_Normal = 13,
        Throw_Flash = 14,
        Frame_Ember = 15,
        Bomb_Shockwave = 16,
        Bomb_PlShockwave = 17,
        Bullet_Default_Ex = 18,
        Invalid = 999,
    };
}
namespace via::nnfc::nfp {
    enum class DetailState : int64_t {
        Init = 0,
        Search = 1,
        Active = 2,
        Deactive = 3,
        Mount = 4,
        Unexpected = 5,
        Invalid = 6,
        UnHandled = 7,
    };
}
namespace app::ropeway::OptionKeyBind {
    enum class ErrorCheckType : int64_t {
        PlayerInGameMenu = 0,
        PauseCommon = 1,
        MapCommon = 2,
        InventoryCommon = 3,
        _Max = 4,
    };
}
namespace via::render {
    enum class GBufferType : int64_t {
        Static = 0,
        Dynamic = 1,
        Transparent = 2,
        TransparentDynamic = 3,
        MAX = 4,
    };
}
namespace app::ropeway::survivor::npc::NpcOrderDefine {
    enum class LookAtCommandKind : int64_t {
        SetDisable = 0,
        SetDeactive = 1,
        ForceTarget = 2,
        SetBodyDirection = 3,
        Invalid = 4,
    };
}
namespace via::render {
    enum class LodMode : int64_t {
        Automatic = 0,
        Manual = 1,
        FollowParent = 2,
        TargetObject = 3,
    };
}
namespace via {
    enum class CameraType : int64_t {
        Game = 0,
        Debug = 1,
        Scene = 2,
        SceneXY = 3,
        SceneYZ = 4,
        SceneXZ = 5,
        Preview = 6,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::SetReactionEnable {
    enum class SET : int64_t {
        ON = 0,
        OFF = 1,
    };
}
namespace via::render {
    enum class SemanticType : int64_t {
        Position = 0,
        Normal = 1,
        Binormal = 2,
        Tangent = 3,
        Texcoord = 4,
        Index = 5,
        Weight = 6,
        Color = 7,
        VertexID = 8,
        Generic = 9,
        InstanceID = 10,
        UniqueUV = 11,
        TessParam = 12,
        GroupID = 13,
    };
}
namespace app::ropeway::gui::RogueInventoryBehavior {
    enum class GimmickMode : int64_t {
        None = 0,
        Use = 1,
        UseDispPlayer = 2,
    };
}
namespace app::ropeway::gimmick::option::MovePositionSettings {
    enum class PositionType : int64_t {
        ONSET_POS = 0,
        TARGET_POS = 1,
        TARGET_OBJ = 2,
        ONSET_OFFSET = 3,
    };
}
namespace app::ropeway::NpcDefine {
    enum class LookAtResidentModeType : int64_t {
        DISABLE = 0,
        LOOK_AT_PLAYER = 1,
        LOOK_AT_NOTICE_POINT = 2,
    };
}
namespace app::ropeway::ObjectUpdateAndDrawRestrictManager {
    enum class ObjectType : int64_t {
        INVALID = 0,
        GUI = 1,
        PLAYER = 2,
        ACTOR = 3,
        ENEMY = 4,
        FSM = 5,
        Item = 6,
        ENVIRONMENTS = 7,
        EFFECTS = 8,
        SCENE_SYSTEM_CONTROL = 9,
        PLAYER_INPUT = 10,
        ROPEWAY_CONTENTS = 11,
        MENU_PAUSE = 12,
        GUI_MASTER = 13,
        GUI_TITLE = 14,
        GUI_INGAME = 15,
        EVENT = 16,
        DYNAMICS = 17,
        LEVEL = 18,
        NUM = 19,
    };
}
namespace via::str {
    enum class SplitOptionType : int64_t {
        None = 0,
        RemoveEmptyEntries = 1,
    };
}
namespace app::ropeway::camera {
    enum class CameraInterpolateMode : int64_t {
        LINEAR = 0,
        HERMITE = 1,
        HELIX = 2,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class BodyParts : int64_t {
        Head = 0,
        Chest = 1,
        Stomach = 2,
        Shoulder = 3,
        UpperArm = 4,
        Forearm = 5,
        Hand = 6,
        Thigh = 7,
        ShIN_ = 8,
        Foot = 9,
        Unique00 = 10,
        Unique01 = 11,
    };
}
namespace via::hid::VibrationResource {
    enum class VibrationResourceType : int64_t {
        DirectMotor = 0,
        WaveFile = 1,
    };
}
namespace app::ropeway::gui::RogueInventoryDetailBehavior {
    enum class ItemDetailState : int64_t {
        NoMove = 0,
        Wait = 1,
        Start = 2,
        Enter = 3,
        Move = 4,
        WaitDefault = 5,
        Leave = 6,
        Angle = 7,
        PlayMotion = 8,
        WaitMotion = 9,
        Action = 10,
        WaitText = 11,
        Change = 12,
        ChangeName = 13,
        ChangeOut = 14,
        ChangeWait = 15,
        ChangeIN_ = 16,
        ChangeEnd = 17,
        PuzzleStart = 18,
        PuzzleWait = 19,
        PuzzleDemo = 20,
        PuzzleEnd = 21,
        Gimmick = 22,
        GetKeyItem_IN_ = 23,
        GetKeyItem_Move = 24,
        GetKeyItem_Update = 25,
        GetKeyItem_Wait = 26,
        CombineStart = 27,
        CombinePlayMotion = 28,
        CombineWaitMotion = 29,
        CombineChangeOut = 30,
        CombineChangeWait = 31,
        CombineChangeIN_ = 32,
        CombineChangeEnd = 33,
    };
}
namespace via::motion {
    enum class ChainType : int64_t {
        ChaIN_ = 0,
        Shooter = 1,
    };
}
namespace app::ropeway::enemy::em6000::param::FlagChecker {
    enum class SWITCH_TYPE : int64_t {
        ON = 0,
        OFF = 1,
    };
}
namespace via::render::detail {
    enum class PrimitiveGUIFilter : int64_t {
        None = 0,
        InstantBlur = 1,
        GaussianBlur = 2,
    };
}
namespace app::ropeway::weapon::shell::ShellDefine {
    enum class BombShellType : int64_t {
        Explosion = 0,
        Blow = 1,
        Shockwave = 2,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::EmCommonFsmAction_SetGroundState {
    enum class EXEC_TYPE : int64_t {
        START = 0,
        END = 1,
        FRAME = 2,
        SEQUENCE = 3,
    };
}
namespace app::ropeway::gamemastering::GameOverFlow {
    enum class State : int64_t {
        INITIALIZE = 0,
        FADE_IN_ = 1,
        FADE_OUT = 2,
        CONTINUE = 3,
        CONTINUE_WAIT = 4,
        CONTINUE_DEVELOP = 5,
        LOAD = 6,
        TO_TITLE = 7,
        TO_INGAME = 8,
        TO_TITLE_1 = 9,
        CONTINUE_1 = 10,
        TO_ROGUE_BASEAREA = 11,
        TO_ROGUE_INGAME = 12,
        TO_ROGUE_RESULT = 13,
        TO_ROGUE_CHARASELECT = 14,
        WAIT_ROGUE_CHARASELECT = 15,
        TO_LMODE_INGAME = 16,
        WAIT_GUI_ANIMATION = 100,
        INITIALIZE_SIMPLE = 101,
        INITIALIZE_WHITE = 102,
        INITIALIZE_WHITE_OUT = 103,
        WAIT_WHITE_OUT = 104,
    };
}
namespace app::ropeway::gui::RogueInventoryDetailBehavior {
    enum class PuzzleDemoState : int64_t {
        NoMove = 0,
        PortableSafeStartDemo = 1,
        PortableSafeWaitDemo = 2,
        PortableSafeAngle = 3,
        PortableSafePlayMotion = 4,
        PortableSafeWaitMotion = 5,
        CarKeyStartDemo = 6,
        CarKeyWaitDemo = 7,
    };
}
namespace via::render {
    enum class SparseShadowTreeResolution : int64_t {
        SparseShadowTreeResolution_4K = 4,
        SparseShadowTreeResolution_8K = 8,
        SparseShadowTreeResolution_16K = 16,
        SparseShadowTreeResolution_32K = 32,
        SparseShadowTreeResolution_64K = 64,
        SparseShadowTreeResolution_AUTO = 0,
    };
}
namespace via::motion::IkDog {
    enum class FootLockMode : int64_t {
        Auto = 0,
        SemiAuto = 1,
    };
}
namespace app::ropeway::LookAtDefine {
    enum class LimitAngleKind : int64_t {
        Rectangle = 0,
        Ellipse = 1,
    };
}
namespace via::motion::detail::MotionFsm2TransitionData {
    enum class EndType : int64_t {
        None = 0,
        EndOfMotion = 1,
        ExitFrame = 2,
        ExitFrameFromEnd = 3,
        SyncPoint = 4,
        SyncPointFromEnd = 5,
    };
}
namespace app::ropeway {
    enum class ThreeFootLockBitFlag : int64_t {
        LEFT = 1,
        RIGHT = 2,
        THIRD = 4,
    };
}
namespace via::gui {
    enum class EffectAlphaType : int64_t {
        Independence = 0,
        MultiplyTextColorAlpha = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickBreakableColumnForG2Battle {
    enum class RigidBodyStateCheckState : int64_t {
        Initialize = 0,
        Static = 1,
        WaitNotSleep = 2,
        WaitSleep = 3,
        Sleep = 4,
    };
}
namespace app::ropeway::OptionManager {
    enum class AppLanguage : int64_t {
        JAPANESE = 0,
        ENGLISH = 1,
        FRENCH = 2,
        ITALIAN = 3,
        GERMAN = 4,
        SPANISH = 5,
        RUSSIAN = 6,
        POLISH = 7,
        CHINESE_TRASITIONAL = 8,
        CHINESE_SIMPLELIFIED = 9,
        KOREAN = 10,
        BRAZILIAN_PORTUGUESE = 11,
    };
}
namespace app::ropeway::fsmv2::enemy::em7000 {
    enum class ACT_WALK_G_TAG : int64_t {
        Walk = 2991702624,
        Run = 161703095,
    };
}
namespace app::ropeway::enemy::em5000::ActionStatus {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_MOVE = 1,
        ACT_ATTENTION = 2,
        ACT_WAKEUP = 3,
        ACT_FAKEDEAD = 4,
        ATK_HOLD = 2000,
        ATK_HOLD_BURN = 2001,
        ATK_GRAPPLE = 2002,
        DMG_STUN = 3000,
        DMG_TOTTER = 3001,
        DMG_KNOCKBACK = 3002,
        DMG_BREAK_WEAK_PARTS = 3003,
        DMG_BURN = 3004,
        DMG_SHOCK = 3005,
        DMG_SHOCK_DOWN = 3006,
        SPT_KNIFE = 4000,
        SPT_GRENADE = 4001,
        DIE_NORMAL = 5000,
        DIE_RIGID = 5001,
        SET_IDLE = 6000,
        SET_HANG = 6001,
        SET_DEAD_IDLE = 6002,
        SET_DEAD = 6003,
        SET_FAKE_DEAD = 6004,
        INVALID_ACTION_ID = 0xFFFFFFFF,
    };
}
namespace via::gui {
    enum class ViewType : int64_t {
        Screen = 0,
        World = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickClockWork {
    enum class ActionResult : int64_t {
        NOT_YET = 0,
        END = 1,
        FAILED = 2,
    };
}
namespace via::os {
    enum class MemoryBus : int64_t {
        WB = 256,
        WC = 512,
        Default = 256,
    };
}
namespace app::ropeway::network {
    enum class StoreType : int64_t {
        Normal = 0,
    };
}
namespace app::ropeway::weapon::generator::BombGeneratorUserDataBase{
    enum class BombBiteGenerateSetting :  int64_t {
        S = 0,
        M = 1,
        L = 2,
    };
}
namespace app::ropeway::gui::GUIMaster::GuiInput {
    enum class KeyRepeat : int64_t {
        NONE = 0,
        MOVE_LEFT = 1,
        MOVE_RIGHT = 2,
        MOVE_UP = 3,
        MOVE_DOWN = 4,
        SHIFT_L1 = 5,
        SHIFT_R1 = 6,
        SHIFT_L2 = 7,
        SHIFT_R2 = 8,
    };
}
namespace app::ropeway::gimmick::action::InterpolationUtil {
    enum class RotateType : int64_t {
        AUTO = 0,
        PLUS = 1,
        MINUS = 2,
    };
}
namespace app::ropeway::enemy::em7100::fsmv2::action::MotionCameraPlayRequest {
    enum class CameraTarget : int64_t {
        CurrentPlayer = 0,
        Owner = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickPressed {
    enum class HoldMoveSide : int64_t {
        PUSH = 0,
        PULL = 1,
        LEFT = 2,
        RIGHT = 3,
        MAX = 4,
    };
}
namespace app::ropeway {
    enum class EnemyActionPriority : int64_t {
        Priority_0000 = 0,
        Priority_0100 = 100,
        Priority_0200 = 200,
        Priority_0300 = 300,
        Priority_0400 = 400,
        Priority_0500 = 500,
        Priority_0600 = 600,
        Priority_0700 = 700,
        Priority_0800 = 800,
        Priority_0900 = 900,
        Priority_1000 = 1000,
        Priority_1100 = 1100,
        Priority_1200 = 1200,
        Priority_1300 = 1300,
        Priority_1400 = 1400,
        Priority_1500 = 1500,
        Priority_1600 = 1600,
        Priority_1700 = 1700,
        Priority_1800 = 1800,
        Priority_1900 = 1900,
        Priority_2000 = 2000,
        Priority_2100 = 2100,
        Priority_2200 = 2200,
        Priority_2300 = 2300,
        Priority_2400 = 2400,
        Priority_2500 = 2500,
        Priority_2600 = 2600,
        Priority_2700 = 2700,
        Priority_2800 = 2800,
        Priority_2900 = 2900,
        Priority_3000 = 3000,
        Priority_3100 = 3100,
        Priority_3200 = 3200,
        Priority_3300 = 3300,
        Priority_3400 = 3400,
        Priority_3500 = 3500,
        Priority_3600 = 3600,
        Priority_3700 = 3700,
        Priority_3800 = 3800,
        Priority_3900 = 3900,
        Priority_4000 = 4000,
        Priority_4100 = 4100,
        Priority_4200 = 4200,
        Priority_4300 = 4300,
        Priority_4400 = 4400,
        Priority_4500 = 4500,
        Priority_4600 = 4600,
        Priority_4700 = 4700,
        Priority_4800 = 4800,
        Priority_4900 = 4900,
        Priority_5000 = 5000,
        Priority_5100 = 5100,
        Priority_5200 = 5200,
        Priority_5300 = 5300,
        Priority_5400 = 5400,
        Priority_5500 = 5500,
        Priority_5600 = 5600,
        Priority_5700 = 5700,
        Priority_5800 = 5800,
        Priority_5900 = 5900,
        Priority_6000 = 6000,
        Priority_6100 = 6100,
        Priority_6200 = 6200,
        Priority_6300 = 6300,
        Priority_6400 = 6400,
        Priority_6500 = 6500,
        Priority_6600 = 6600,
        Priority_6700 = 6700,
        Priority_6800 = 6800,
        Priority_6900 = 6900,
        Priority_7000 = 7000,
        Priority_7100 = 7100,
        Priority_7200 = 7200,
        Priority_7300 = 7300,
        Priority_7400 = 7400,
        Priority_7500 = 7500,
        Priority_7600 = 7600,
        Priority_7700 = 7700,
        Priority_7800 = 7800,
        Priority_7900 = 7900,
        Priority_8000 = 8000,
        Priority_8100 = 8100,
        Priority_8200 = 8200,
        Priority_8300 = 8300,
        Priority_8400 = 8400,
        Priority_8500 = 8500,
        Priority_8600 = 8600,
        Priority_8700 = 8700,
        Priority_8800 = 8800,
        Priority_8900 = 8900,
        Priority_9000 = 9000,
        Priority_9100 = 9100,
        Priority_9200 = 9200,
        Priority_9300 = 9300,
        Priority_9400 = 9400,
        Priority_9500 = 9500,
        Priority_9600 = 9600,
        Priority_9700 = 9700,
        Priority_9800 = 9800,
        Priority_9900 = 9900,
        Priority_9999 = 9999,
        Invalid = 0xFFFFFFFF,
    };
}
namespace app::ropeway::enemy::em7300::Em7300Think {
    enum class CheckID : int64_t {
        ATK_DASH = 0,
        ATK_RAGE = 1,
        ATK_HIGH_R = 2,
        ATK_MIDL_TWO = 3,
        ATK_RAGE_2 = 4,
        ATK_BOTH_HAND = 5,
        ATK_BACK = 6,
        ATK_LANE_L = 7,
        ATK_LANE_R = 8,
        ACT_BACKSTEP = 9,
        NoSet = 10,
    };
}
namespace app::ropeway::EmBurnupController {
    enum class BurnPartList : int64_t {
        ArmL = 0,
        ArmR = 1,
        LegL = 2,
        LegR = 3,
        Head = 4,
        Body = 5,
        Waist = 6,
    };
}
namespace via::hid::VrTracker {
    enum class ResultType : int64_t {
        Predicted = 0,
        Raw = 1,
        Default = 0,
    };
}
namespace via::gui {
    enum class MaterialParamType : int64_t {
        Unknown = 0,
        Float = 1,
        Float4 = 2,
        Color = 3,
        Texture = 4,
    };
}
namespace via::browser {
    enum class ServiceType : int64_t {
        None = 0,
        Steam = 1,
        Psn = 2,
        Live = 3,
        Max = 4,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class HoldTargetStatus : int64_t {
        IDLE = 0,
        MOVE = 1,
        TOWARD = 2,
        SIGHTING = 3,
        GRAPPLE = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickWordBlock::AnimationData {
    enum class StateType : int64_t {
        WAIT = 0,
        PUT = 1,
        PICKED = 2,
    };
}
namespace app::ropeway::fsmv2::enemy::em7000 {
    enum class THINK_TAG : int64_t {
        DISABLE_CHECK_RANGE = 93519404,
        DISABLE_ARM_SCALE = 1033952981,
        DISABLE_EYE_LOOK_AT = 4027191138,
        DISABLE_ARM_SCALE_MOTION = 2898327988,
        DISABLE_EYE_MOTION = 991946528,
    };
}
namespace via::hid {
    enum class DeviceKind : int64_t {
        Unknown = 0,
        GamePad = 1,
        Keyboard = 2,
        Mouse = 3,
        Camera = 4,
        HMD = 5,
        TouchScreen = 6,
        VirtualKeyboard = 7,
    };
}
namespace JsonParser {
    enum class ValueType : int64_t {
        Unknown = 0,
        Object = 1,
        Array = 2,
        String = 3,
        Number = 4,
        TRUE_ = 5,
        FALSE_ = 6,
        NULL_ = 7,
    };
}
namespace via::gui::MessageAnalyzer {
    enum class Error : int64_t {
        None = 0,
        TagAlloc = 1,
        InvalidChar = 2,
        InvalidFont = 4,
        InvalidTag = 8,
        RangeOverW = 16,
        RangeOverH = 32,
        InvalidState = 64,
    };
}
namespace via::os {
    enum class IpAddressType : int64_t {
        Default = 0,
        V4 = 1,
        V6 = 2,
    };
}
namespace app::ropeway::gui::RogueCharaSelectBehavior {
    enum class OpenMode : int64_t {
        INVALID = 0xFFFFFFFF,
        CHARA_SELECT = 0,
        LOAD = 1,
        NO_LOAD = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickShutterG2Break {
    enum class BreakType : int64_t {
        Break_None = 0xFFFFFFFF,
        Normal = 0,
        Break_01 = 1,
        Break_02 = 2,
        BreakTypeNum = 3,
    };
}
namespace app::ropeway::behaviortree::action::survivor::npc::NpcBtCondition_Vital {
    enum class CheckKind : int64_t {
        Equal = 0,
        NotEqual = 1,
    };
}
namespace via::motion::IkDamageAction {
    enum class Calculation : int64_t {
        Default = 0,
        AddDirection = 1,
    };
}
namespace app::ropeway::gui::FloorMapSt12Behavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        Door_5_1_001 = 1,
        Door_5_1_005 = 2,
        Door_5_1_011 = 3,
        Door_5_1_012 = 4,
    };
}
namespace via::render::LightProbes {
    enum class LightProbesMerge : int64_t {
        Disable = 0,
        A = 1,
        B = 2,
        AB = 3,
        C = 4,
        AC = 5,
        BC = 6,
        ABC = 7,
        D = 8,
        E = 16,
        F = 32,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class ForceMoveType : int64_t {
        Auto = 0,
        Walk = 1,
        Caution = 2,
        Run = 3,
        Unique = 4,
    };
}
namespace via {
    enum class ShareServiceCaps : int64_t {
        None = 0,
        ScreenShot = 1,
        ScreenShotControl = 2,
        ScreenShotRequest = 4,
        ScreenShotOverlayImage = 8,
        ScreenShotChangeOverlayImage = 16,
        VideoRecording = 32,
        VideoRecordingControl = 64,
        VideoRecordingRequest = 128,
        VideoRecordingOverlayImage = 256,
        VideoRecordingChangeOverlayImage = 512,
        GameLiveStreaming = 1024,
        GameLiveStreamingControl = 2048,
        GameLiveStreamingRequest = 4096,
        GameLiveStreamingStatusWatching = 8192,
        SharePlay = 16384,
        SharePlayControl = 32768,
        SharePlayStatusWatching = 65536,
        ShareUtility = 131072,
        ScreenShotShowFiles = 262144,
    };
}
namespace app::ropeway::weapon::generator::BombGeneratorUserDataBase{
    enum class BombBulletGrenadeGenerateSetting :  int64_t {
        S = 0,
        M = 1,
        L = 2,
    };
}
namespace app::ropeway::TerrainAnalyzer {
    enum class AnalyzeType : int64_t {
        STEP = 0,
        LINTEL = 1,
    };
}
namespace app::ropeway::Em4000Define {
    enum class DifficultyType : int64_t {
        Normal = 0,
        Hard = 1,
    };
}
namespace via::hid::virtualKeyboard::nsw {
    enum class PasswordMode : int64_t {
        Show = 0,
        Hide = 1,
    };
}
namespace via::render {
    enum class ShadowResolution : int64_t {
        Lowest = 384,
        Low = 512,
        Normal = 768,
        High = 1024,
        Highest = 2048,
        Ultra = 4096,
    };
}
namespace app::ropeway::enemy::common::fsmv2::condition::EnemyTargetDirection {
    enum class TYPE : int64_t {
        IsLeft = 0,
        RangeIN_ = 1,
        RangeOver = 2,
    };
}
namespace via::motion::MotionBankDataSolver {
    enum class SetupState : int64_t {
        None = 0,
        Reload = 1,
        Setuped = 2,
    };
}
namespace via::navigation::FilterInfo {
    enum class TraceDestination : int64_t {
        Optimize = 0,
        PortalCenter = 1,
        NodeCenterWithExtraLink = 2,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class SurvivorType : int64_t {
        Invalid = 0xFFFFFFFF,
        PL0000 = 0,
        PL1000 = 1,
        PL2000 = 2,
        PL3000 = 3,
        PL4000 = 4,
        PL4100 = 5,
        PL5000 = 6,
        PL5100 = 7,
        PL5200 = 8,
        PL5300 = 9,
        PL5400 = 10,
        PL5500 = 11,
        PL5600 = 12,
        PL5700 = 13,
        PL5800 = 14,
        PL5900 = 15,
        PL6000 = 16,
        PL6100 = 17,
        PL6200 = 18,
        PL6300 = 19,
        PL6400 = 20,
        PL6500 = 21,
        PL6600 = 22,
        PL6700 = 23,
        PL6800 = 24,
        PL6900 = 25,
        PL7000 = 26,
        PL7100 = 27,
        PL7200 = 28,
        PL7300 = 29,
        PL7400 = 30,
        PL7500 = 31,
        PL7600 = 32,
        PL7700 = 33,
        PL7800 = 34,
        PL7900 = 35,
        PL8000 = 36,
        PL8100 = 37,
        PL8200 = 38,
        PL8300 = 39,
        PL8400 = 40,
        PL8500 = 41,
        PL8600 = 42,
        PL8700 = 43,
        PL8800 = 44,
        PL8900 = 45,
        PL9000 = 46,
        PL9100 = 47,
        PL9200 = 48,
        PL9300 = 49,
        PL9400 = 50,
        PL9500 = 51,
        PL9600 = 52,
        PL9700 = 53,
        PL9800 = 54,
        PL9900 = 55,
    };
}
namespace app::ropeway::EnemyGimmickConfiscateController {
    enum class STATE : int64_t {
        Move = 0,
        GmcLocking = 1,
        GmcAccessing = 2,
        GmcKeep = 3,
        GmcKeepLocking = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickAutoDoorOptions::Setting {
    enum class SideType : int64_t {
        BOTH = 0,
        SIDE_A_ONLY = 1,
        SIDE_B_ONLY = 2,
    };
}
namespace via::motion::CameraAnimation {
    enum class AnimType : int64_t {
        Trans = 1,
        Quat = 2,
        Fov = 4,
        Dist = 8,
    };
}
namespace app::ropeway::enemy::em6000::tracks::Em6000HoldDamageTrack {
    enum class HoldDamageType : int64_t {
        HOLD = 0,
    };
}
namespace via::clr {
    enum class ParamFlag : int64_t {
        IN_ = 1,
        Out = 2,
        Optional = 16,
        HasDefault = 4096,
        HasFieldMarshal = 8192,
        ByRef = 16384,
        Ptr = 32768,
    };
}
namespace via::render {
    enum class FenceState : int64_t {
        Invalidate = 0xFFFFFFFF,
        DoNotWait = 4294967294,
        UAVSync = 4294967293,
    };
}
namespace via::navigation::PathObjectOperator {
    enum class ForwardResult : int64_t {
        None = 0,
        Forward = 1,
        End = 2,
    };
}
namespace app::ropeway::enemy::tracks::Em7200BoxThrowTrack {
    enum class ThrowStateEnum : int64_t {
        NONE = 0,
        INIT = 1,
        START = 2,
        THROW = 3,
        END = 4,
        DIRSET = 5,
    };
}
namespace via::network::storage {
    enum class Type : int64_t {
        None = 0,
        Title = 1,
        User = 2,
    };
}
namespace via::network::wrangler {
    enum class ProviderLatency : int64_t {
        Undefined = 0,
        Normal = 1,
        RealTime = 2,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class FeatherMotion : int64_t {
        IDLE_TYPE_A = 0,
        IDLE_TYPE_B = 1,
        IDLE_TYPE_C = 2,
        IDLE_TYPE_D = 3,
        IDLE_TYPE_E = 4,
        IDLE_TYPE_F = 5,
        IDLE_LOOK_UP = 6,
        KNOCK_HEAD_00 = 7,
        KNOCK_WALL_00 = 8,
        KNOCK_FENCE_00 = 9,
        KNOCK_PRISON_00 = 10,
        KNOCK_STEP_00 = 11,
        EAT_00 = 12,
        DEAD_SIT_00 = 13,
        DEAD_SIT_01 = 14,
        DEAD_SIT_02 = 15,
        DEAD_SIT_CHAIR_00 = 16,
        DEAD_SIT_CHAIR_01 = 17,
        DEAD_SIT_CHAIR_02 = 18,
        DEAD_SIT_WALLSIDE_00 = 19,
        DEAD_SIT_WALLSIDE_01 = 20,
        DEAD_FACEDOWN_00 = 21,
        DEAD_FACEDOWN_01 = 22,
        DEAD_FACEDOWN_02 = 23,
        DEAD_FACEUP_00 = 24,
        DEAD_FACEUP_01 = 25,
        DEAD_FACEUP_02 = 26,
    };
}
namespace via::render {
    enum class HazeCompositorResolution : int64_t {
        Low = 0,
        Mid = 1,
        High = 2,
    };
}
namespace via::motion::IkDamageAction {
    enum class CalculationBendRotation : int64_t {
        Parent = 0,
        Root = 1,
    };
}
namespace via::hid::mouse {
    enum class ManipulatorClientDefaultTypeDev : int64_t {
        WindowMessage = 3,
        GlobalParameter = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickTargetPracticeOwner {
    enum class TargetState : int64_t {
        DEAD = 0,
        FRONT_SIDE = 1,
        TURNED_SIDE = 2,
    };
}
namespace app::ropeway::enemy::em3000::behaviortree::condition::Em3000BtCondition_CanJump {
    enum class CheckType : int64_t {
        ALL = 0,
        CEILING = 1,
        WALL = 2,
    };
}
namespace via::behaviortree {
    enum class ExecLine : int64_t {
        LINE0 = 0,
        LINE1 = 1,
        LINE2 = 2,
    };
}
namespace via::effect::lensflare {
    enum class TranslationType : int64_t {
        Free = 0,
        Horizontal = 1,
        Vertical = 2,
        Custom = 3,
    };
}
namespace via::userdata {
    enum class TypeKind : int64_t {
        Unknown = 0,
        Enum = 1,
        Boolean = 2,
        Int8 = 3,
        Uint8 = 4,
        Int16 = 5,
        Uint16 = 6,
        Int32 = 7,
        Uint32 = 8,
        Int64 = 9,
        Uint64 = 10,
        Single = 11,
        Double = 12,
        C8 = 13,
        C16 = 14,
        String = 15,
        Trigger = 16,
        Vec2 = 17,
        Vec3 = 18,
        Vec4 = 19,
        Matrix = 20,
        GUID = 21,
        Num = 22,
    };
}
namespace via::motion::IkLeg {
    enum class EffectorCtrl : int64_t {
        None = 0,
        LocalOffset = 1,
        WorldOffset = 2,
        Local = 3,
        World = 4,
    };
}
namespace app::ropeway::player::tag {
    enum class WaterResistanceAttribute : int64_t {
        NONE = 1187282292,
        LOW = 3277248676,
        MIDDLE = 1712691171,
        HIGH = 2952374375,
    };
}
namespace app::ropeway::gamemastering::Map {
    enum class AreaFlag : int64_t {
        None = 0,
        _0 = 1,
        _1 = 2,
        _2 = 4,
        _3 = 8,
        _4 = 16,
        _5 = 32,
        ALL = 63,
    };
}
namespace app::ropeway::IkAdjustLimb {
    enum class SolvePhase : int64_t {
        LateUpdate = 0,
        AfterIkController = 1,
        AfterRagdollController = 2,
        ByBehavior = 3,
    };
}
namespace via::render::LightProbes {
    enum class LightProbesType : int64_t {
        Indoor = 0,
        Outdoor = 1,
        Sparse = 2,
    };
}
namespace via::navigation::FilterInfo {
    enum class FilterType : int64_t {
        Cost = 0,
        NotWalkable = 1,
        Walkable = 2,
        Through = 3,
    };
}
namespace app::ropeway::PrefabReferer {
    enum class LoadStatus : int64_t {
        STANDBY = 0,
        LOADING = 1,
        READY = 2,
        UNLOADING = 3,
    };
}
namespace app::ropeway::OptionKeyBind {
    enum class KeyBindGroupType : int64_t {
        PlayerCommand = 0,
        InGameOpenMenu = 1,
        CommonUICommand = 2,
        MenuMoveCommand = 3,
        MenuPageCommand = 4,
        ItemBoxCommand = 5,
        MapFigureCommand = 6,
    };
}
namespace via::render::Stamp {
    enum class RasterMode : int64_t {
        Standard = 0,
        Wireframe = 2,
        NeighborSample = 3,
    };
}
namespace via::movie {
    enum class BlendMode : int64_t {
        Normal = 0,
        Add = 1,
    };
}
namespace via::effect::lensflare {
    enum class RotationOrder : int64_t {
        XYZ = 0,
        XZY = 1,
        YXZ = 2,
        YZX = 3,
        ZXY = 4,
        ZYX = 5,
    };
}
namespace app::ropeway::gimmick::action::TriggerArea {
    enum class CollisionUserDataType : int64_t {
        ANY = 0,
        TYPE_A = 1,
        TYPE_B = 2,
        TYPE_C = 3,
        TYPE_D = 4,
        TYPE_E = 5,
        TYPE_F = 6,
        TYPE_G = 7,
    };
}
namespace app::ropeway::gui::RogueAccessoryBehavior {
    enum class UpdateState : int64_t {
        NONE = 0,
        INPUT = 1,
        WAIT_SAVE = 2,
        TO_CLOSE = 3,
        CLOSE = 4,
        END = 5,
    };
}
namespace via::str {
    enum class SplitSeparatorType : int64_t {
        String = 0,
        CharArray = 1,
    };
}
namespace via::physics {
    enum class ShapeCastOption : int64_t {
        AllHits = 0,
        DisableBackFacingTriangleHits = 1,
        DisableFrontFacingTriangleHits = 2,
        BackFacingTriangleHits = 3,
        FrontFacingTriangleHits = 4,
        NearSort = 5,
        OneHitBreak = 6,
        Max = 7,
    };
}
namespace via::hid::hmd::MorpheusDevice {
    enum class FovType : int64_t {
        DeviceDefault = 0,
        SystemOverride = 1,
    };
}
namespace via::motion::detail {
    enum class ChainDebugDisplayMode : int64_t {
        HighLight = 0,
        SelectOnly = 1,
        None = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickWindow {
    enum class DispType : int64_t {
        BROKEN_OR_WAIT = 0,
        BREAKING = 1,
    };
}
namespace via::render::VolumeDecal {
    enum class ValidFlag : int64_t {
        None = 0,
        BaseColor = 1,
        Normal = 2,
        Roughness = 4,
        Emissive = 8,
        AlphaMask = 16,
        AlphaSecondMask = 32,
        NormalRoughness = 64,
        All = 127,
        FalloffFromTwoside = 2147483648,
    };
}
namespace app::ropeway::enemy::common::behaviortree::condition::EmCommonBtCondition_PlayerMove {
    enum class PlayerMoveType : int64_t {
        Stop = 0,
        Approach = 1,
        Leave = 2,
        StopOrApproach = 3,
        StopOrLeave = 4,
    };
}
namespace via::navigation::ObstacleFilterInfo {
    enum class FilterType : int64_t {
        Avoid = 0,
        Through = 1,
    };
}
namespace app::ropeway::EquipmentDefine {
    enum class WeaponParts : int64_t {
        None = 0,
        A = 1,
        B = 2,
        C = 4,
        FullCustom = 7,
    };
}
namespace System {
    enum class Base64FormattingOptions : int64_t {
        None = 0,
        InsertLineBreaks = 1,
    };
}
namespace via::motion {
    enum class RootPlayMode : int64_t {
        None = 0,
        Fixed = 1,
        Continuance = 2,
        Joint = 3,
        FixedWithScale = 4,
    };
}
namespace via::motion {
    enum class ExitType : int64_t {
        None = 0,
        End = 1,
        Frame = 2,
        FrameFromEnd = 3,
    };
}
namespace via::gui {
    enum class BarVisibility : int64_t {
        Always = 0,
        AutoHideBar = 1,
        AutoHideAll = 2,
    };
}
namespace app::ropeway::gamemastering::MainFlowManager {
    enum class InterludeEnable : int64_t {
        TRUE_ = 0,
        FALSE_ = 1,
    };
}
namespace via::clr {
    enum class EnumI2 : int64_t {
        Dummy = 0,
    };
}
namespace app::ropeway::WaterSurfaceChecker {
    enum class WATER_SWITCH : int64_t {
        FALSE_ = 0,
        TRUE_ = 1,
    };
}
namespace via::network::AutoMatchmaking {
    enum class RuleCategory : int64_t {
        Group = 0,
        Region = 1,
        P2pVersion = 2,
        Custom = 3,
    };
}
namespace via::effect::detail {
    enum class EmitterDimType : int64_t {
        EmitterDimType_2D = 0,
        EmitterDimType_3D = 1,
    };
}
namespace via::motion::detail::IkNboneSolver {
    enum class calcType : int64_t {
        Basic = 0,
        Heavy = 1,
    };
}
namespace via::render {
    enum class RenderDeviceAPI : int64_t {
        DirectX11 = 0,
        DirectX12 = 1,
        OpenGL = 2,
        OpenGLES = 3,
        Vulkan = 4,
        Mantle = 5,
        Metal = 6,
        Glide = 7,
        DirectX11x = 8,
        DirectX12x = 9,
        GNM = 10,
        NVN = 11,
        Unkown = 12,
        NULL_ = 13,
    };
}
namespace app::ropeway::enemy::em6000::MotionPattern {
    enum class InWater : int64_t {
        Water = 0,
        Ground = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickAction {
    enum class ScenarioType : int64_t {
        ANY = 0,
        LEON = 1,
        CLAIRE = 2,
        LEON_CLAIRE = 3,
    };
}
namespace app::ropeway::timeline::TimelineEventSettingTable::CameraSetting {
    enum class EndCameraState : int64_t {
        INTERPOLATE = 0,
        CONTINUANCE = 1,
    };
}
namespace app::ropeway::gamemastering::UIMapManager {
    enum class DoorState : int64_t {
        NOT_CHECKED = 0,
        LOCKED = 1,
        UNLOCKED = 2,
        WIPED = 3,
        LOCKED_ITEM = 4,
        BROKEN = 5,
        ONEWAY = 6,
        ONEWAY_REV = 7,
        UNLOCKED_ITEM = 8,
    };
}
namespace app::ropeway::OptionManager {
    enum class ID : int64_t {
        ControllerObjectiveCameraSub = 0,
        ControllerSubjectiveCameraSub = 1,
        ControllerVibration = 2,
        ControllerAutoReload = 3,
        ControllerHoldOption = 4,
        ControllerHoldButton = 5,
        CameraNormalReverse = 6,
        CameraHoldReverse = 7,
        CameraNormalSpeed = 8,
        CameraHoldSpeed = 9,
        CameraNormalAcceleration = 10,
        CameraHoldAcceleration = 11,
        CameraVibration = 12,
        DisplayBrightness = 13,
        DisplayCaption = 14,
        DisplayLaserColor = 15,
        DisplayTutorial = 16,
        AudioListeningMode = 17,
        AudioDRControl = 18,
        AudioBGMVolume = 19,
        AudioSEVolume = 20,
        AudioVoiceVolume = 21,
        LanguageCaption = 22,
        LanguageVoice = 23,
        LanguageText = 24,
        ReNetOnOff = 25,
        ReNetData = 26,
        ControllerOldCameraMoveAdjust = 27,
        ControllerNewCameraMoveAdjust = 28,
        ControllerOldEnemyLookAt = 29,
        ControllerNewEnemyLookAt = 30,
        ControllerOldCameraLockOn = 31,
        ControllerNewCameraLockOn = 32,
        ControllerOldAttackType = 33,
        ControllerNewAttackType = 34,
        CameraAimAssistLevel = 35,
        CameraAimCorrectAngle = 36,
        SafeAreaRatio = 37,
        DisplayBrightnessMax = 38,
        DisplayBrightnessMIN_ = 39,
        DisplayBrightnessHDR = 40,
        DisplayBrightnessHDRMax = 41,
        DisplayReticleColor = 42,
        DisplayUI = 43,
        DisplayColorAssist = 44,
        ControllerButtonType = 45,
        ControllerButtonType_Vita = 46,
        CameraAoV = 47,
        AudioBinaural = 48,
        AudioControllerSpeaker = 49,
        DisplayHDRMode = 50,
        ControllerRunType = 51,
        DisplaySpeedMode = 52,
        TMP_0 = 53,
        TMP_1 = 54,
        TMP_2 = 55,
        TMP_3 = 56,
        TMP_4 = 57,
        TMP_5 = 58,
        TMP_6 = 59,
        TMP_7 = 60,
        PCMouseReverse = 61,
        PCMouseBind = 62,
        PCCameraNormalSpeed = 63,
        PCCameraHoldSpeed = 64,
        PCGraphicsColorSpace = 65,
        PCGraphicsWindowMode = 66,
        PCGraphicsResolution = 67,
        PCGraphicsRefreshrate = 68,
        PCGraphicsFrameRate = 69,
        PCGraphicsVerticalSync = 70,
        PCGraphicsRendering = 71,
        PCGraphicsImageQuality = 72,
        PCGraphicsTextureQuality = 73,
        PCGraphicsTextureFilterQuality = 74,
        PCGraphicsMeshQuality = 75,
        PCGraphicsAntiAlias = 76,
        PCGraphicsMotionBlur = 77,
        _PC_DEL_2 = 78,
        PCGraphicsShadowQuality = 79,
        _PC_DEL_3 = 80,
        PCGraphicsShadowCache = 81,
        PCGraphicsAmbientOcclusion = 82,
        PCGraphicsBloom = 83,
        PCGraphicsLensFlare = 84,
        PCGraphicsVolumeLightQuality = 85,
        PCGraphicsReflection = 86,
        PCGraphicsSubsurfaceScattering = 87,
        PCGraphicsDepthOfField = 88,
        PCGraphicsLensDistortion = 89,
        _PC_DEL_0 = 90,
        PCGraphicsDirectX = 91,
        _PC_DEL_1 = 92,
        PCGraphicsContactShadow = 93,
        PC_0 = 94,
        PC_1 = 95,
        PC_2 = 96,
        PC_3 = 97,
        PC_4 = 98,
        PC_5 = 99,
        PC_6 = 100,
        PC_7 = 101,
        PC_8 = 102,
        PC_9 = 103,
        Max = 104,
    };
}
namespace via::effect::gpgpu::detail {
    enum class NodeBillboardBlendType : int64_t {
        AlphaBlend = 0,
        Physical = 1,
    };
}
namespace app::ropeway::gimmick::option::SurvivorJackActionControlSettings {
    enum class ResultKind : int64_t {
        Success = 0,
        Wait = 1,
        Fail = 2,
        Invalid = 3,
    };
}
namespace via::AnimationCurve3D {
    enum class Wrap : int64_t {
        Once = 0,
        Loop = 1,
        Loop_Always = 2,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class RoleType : int64_t {
        INVALID = 0,
        PLAYER = 1,
        NPC = 2,
        ACTOR = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickLadder {
    enum class ClimbType : int64_t {
        UP = 0,
        DOWN = 1,
        MAX = 2,
    };
}
namespace app::ropeway::WwiseFacialController {
    enum class FacialMotionType : int64_t {
        None = 0,
        Strength = 1,
        Damage = 2,
    };
}
namespace app::ropeway::MainSceneUpdateManager {
    enum class InstantiateMode : int64_t {
        FrameHead = 1,
        Immediately = 4,
        FrameTail = 8,
        WaitPrefabReady = 4096,
        NoNullReaction = 8192,
        NoRetry = 16384,
    };
}
namespace via::motion::IkLookAt {
    enum class LookAtState : int64_t {
        None = 0,
        Move = 1,
        Follow = 2,
    };
}
namespace app::Collision::CollisionSystem::Material {
    enum class Terrain : int64_t {
        TmDefault = 0,
    };
}
namespace app::ropeway::gui::RogueKeyFlagsBehavior {
    enum class State : int64_t {
        INVALID = 0xFFFFFFFF,
        INITIALIZE = 0,
        WAIT = 1,
        PRE_MOVE = 2,
        MOVE = 3,
    };
}
namespace app::ropeway::WwiseContactTrigger {
    enum class CastRayDirectionEnum : int64_t {
        Down = 0,
        ObjectDirection = 1,
        ManualDirection = 2,
    };
}
namespace app::ropeway::gamemastering::UIMapManager {
    enum class FloorMapState : int64_t {
        NONE = 0,
        WAIT = 1,
        SUCCESS = 2,
        FAILED = 3,
    };
}
namespace via::physics {
    enum class GateEventType : int64_t {
        None = 0,
        Enter = 1,
        Leave = 2,
    };
}
namespace app::ropeway::weapon::generator {
    enum class SparkType : int64_t {
        Small = 0,
        Large = 1,
    };
}
namespace via::dynamics::internal::BoundingVolumeHierarchy {
    enum class ErrorCode : int64_t {
        None = 0,
        FunctorAbort = 1,
        Max = 2,
    };
}
namespace app::ropeway::enemy::em6200::fsmv2::action::MotionCameraPlayRequest {
    enum class CameraPatternType : int64_t {
        Hold_Start = 0,
        Hold_Back_Start = 1,
        Hold_Kill = 2,
        Support_Knife = 3,
        Support_FlashGrenade = 4,
        Support_Grenade = 5,
        Throw_R = 6,
        Throw_L = 7,
        Down_Crush = 8,
    };
}
namespace app::ropeway::gui::FloorMapSt5CBehavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        AutoDoor05_escp = 1,
        sm40_060_SecurityDoor_U01 = 2,
        sm40_060_SecurityDoor_U02 = 3,
        sm40_060_SecurityDoor_U03 = 4,
        sm40_060_SecurityDoor_U05 = 5,
        sm40_060_SecurityDoor_U06 = 6,
        sm40_060_SecurityDoor_U07 = 7,
        sm40_060_SecurityDoor_U11 = 8,
        sm40_060_SecurityDoor_U13 = 9,
    };
}
namespace app::ropeway::enemy::EnemyWeaponDataUserData::WeaponValueInfoBase {
    enum class SparkShellType : int64_t {
        Bullet = 0,
        Small = 1,
        Large = 2,
    };
}
namespace app::ropeway::gui::FileBehavior {
    enum class PageCursorBit : int64_t {
        DEFAULT = 0,
        FIRST = 1,
        END = 2,
    };
}
namespace via::hid::mouse {
    enum class ManipulatorClientDefaultType : int64_t {
        DirectInput = 1,
        RawInput = 2,
        WindowMessage = 3,
        GlobalParameter = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickMedicineMachine {
    enum class CylinderPointType : int64_t {
        POINT_A = 0,
        POINT_B = 1,
        POINT_C = 2,
        NONE = 3,
    };
}
namespace app::ropeway::LookAtDefine {
    enum class GraphStateKind : int64_t {
        Zero = 0,
        Quadrant1st = 1,
        PiDiv2 = 2,
        Quadrant2nd = 3,
        Pi = 4,
        Quadrant3rd = 5,
        PiPlusPiDiv2 = 6,
        Quadrant4th = 7,
        Pi2 = 8,
    };
}
namespace app::ropeway::fsmv2::ItemGetMenuFromEnemy {
    enum class EndState : int64_t {
        WAIT = 0,
        DECIDE_END = 1,
        CANCEL_END = 2,
    };
}
namespace app::ropeway::gui::GimmickWristTagReaderGuiBehavior {
    enum class StateType : int64_t {
        NONE = 0,
        SCREENSAVER = 1,
        SUCCESS = 2,
        FAILURE = 3,
        BLOCKADE = 4,
        NOPOWER = 5,
    };
}
namespace via::motion {
    enum class PlayState : int64_t {
        Play = 0,
        Pause = 1,
        Stop = 2,
    };
}
namespace app::ropeway {
    enum class NoticeType : int64_t {
        Look = 1,
        Aim = 2,
        Assist = 4,
    };
}
namespace via::gui {
    enum class IconColorType : int64_t {
        None = 0,
        AlphaOnly = 1,
        RGBA = 2,
    };
}
namespace app::ropeway::ParamCurveAnimator::ProcessLight {
    enum class LightType : int64_t {
        AREA = 0,
        POINT = 1,
        DIRECTIONAL = 2,
        IES = 3,
        SPOT = 4,
        LIGHT_PROBES = 5,
    };
}
namespace via::memory {
    enum class DebugMode : int64_t {
        AllocZeroClear = 1,
        AllocFill = 2,
        FreeZeroClear = 4,
        FreeFill = 8,
        BoundCheck = 16,
        FreeCheck = 32,
        EmbededAlloc = 64,
    };
}
namespace app::ropeway::posteffect::inventory::InventoryFilterLabel {
    enum class Slot : int64_t {
        Default = 0,
        Slot_1 = 1,
        Slot_2 = 2,
        Slot_3 = 3,
        Slot_4 = 4,
        Slot_5 = 5,
        Slot_6 = 6,
        Slot_7 = 7,
        Max = 8,
        Invalid = 9,
    };
}
namespace app::ropeway::OptionKeyBind {
    enum class SlotType : int64_t {
        Primary = 0,
        Secondary = 1,
    };
}
namespace via::render::layer::Overlay {
    enum class SegmentOrder : int64_t {
        GUIPostFilter = 0,
        OverlayPostEffect = 62,
        System = 63,
    };
}
namespace app::ropeway::gui::RogueFloatIconBehavior {
    enum class State : int64_t {
        NONE = 0,
        TO_ON = 1,
        DRAWING = 2,
        TO_OFF = 3,
    };
}
namespace app::ropeway::weapon::shell::ShellCartridge {
    enum class STATE : int64_t {
        Invalid = 0,
        Delay = 1,
        Ejected = 2,
        Standby = 3,
        Finish = 4,
    };
}
namespace app::ropeway::gamemastering::TimelineEventManager {
    enum class EventOrder : int64_t {
        Preset = 0,
        Play = 1,
        Cancel = 2,
        Standby = 3,
        Invalid = 4,
    };
}
namespace via::render::ToneMapping {
    enum class NeighborhoodClamp : int64_t {
        AABBClip = 0,
        VarianceClip = 1,
    };
}
namespace app::ropeway::gui::RemainingBulletBehavior {
    enum class DisplayType : int64_t {
        INVALID = 0,
        TYPE0 = 1,
        TYPE1 = 2,
        TYPE2 = 3,
        TYPE3_INFINITY = 4,
        TYPE3_RELOADABLE = 5,
        TYPE4 = 6,
        TYPE5 = 7,
    };
}
namespace app::ropeway::enemy::em3000 {
    enum class RankParamHash : int64_t {
        JUMP_AHEAD_RESTART_TIME_RATE = 560254695,
        SIGN_OF_ATTACK_SPEED_RATE = 1262840115,
        NUMBER_OF_PERSISTENT = 4059898801,
        TARGETED_TO_ESCAPE_TIME_RATE = 3576281015,
    };
}
namespace via::hid::camera {
    enum class PlayStationCameraFormatLevel : int64_t {
        Level0 = 1,
        Level1 = 2,
        Level2 = 4,
        Level3 = 8,
        All = 15,
        LevelMax = 4,
    };
}
namespace via::effect::detail {
    enum class EmitterStateType : int64_t {
        Idle = 0,
        Start = 1,
        Running = 2,
        Stop = 3,
        Kill = 4,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class Phase : int64_t {
        FASTEST = -115,
        HeadSystem = -114,
        Screen = -91,
        Scene = -86,
        Map = -80,
        Positioning = -66,
        EnemyManage = -61,
        EnemyControl = -58,
        PlayerManage = -51,
        PlayerControl = -48,
        CameraControl = -41,
        TailSystem = -31,
        Free = -11,
        LATEST = 120,
    };
}
namespace app::ropeway::weapon::shell::ShellDefine {
    enum class ShellTypeAttribute : int64_t {
        Bullet = 0,
        Bomb = 1,
        Spark = 2,
        Throw = 3,
        Normal = 4,
        Fit = 5,
        Center = 6,
        Arround = 7,
        Explosion = 8,
        Blow = 9,
        Small = 10,
        Large = 11,
        Grenade_Normal = 12,
        Grenade_Flash = 13,
        GrenadeLuncher_Acid = 14,
        GrenadeLuncher_Fire = 15,
        Bited = 16,
        Shockwave = 17,
        Heal = 18,
        Ember = 19,
        Bullet_Ex = 20,
        Poison = 21,
    };
}
namespace app::ropeway::NpcDefine {
    enum class PartnerType : int64_t {
        PL2000 = 0,
        PL3000 = 1,
        Invalid = 2,
    };
}
namespace via::render::SwingWind {
    enum class WindType : int64_t {
        Directional = 0,
        Point = 1,
        Push = 2,
    };
}
namespace app::ropeway::environment::lifecycle::EnvironmentStandbyEventReceivable::Event {
    enum class EventType : int64_t {
        Invalid = 0,
        Standby = 1,
        Release = 2,
        Activate = 3,
        Deactivate = 4,
    };
}
namespace via::motion {
    enum class ConstraintsUpdate : int64_t {
        PrevLateUpdate = 0,
        AfterLateUpdate = 1,
        Last = 2,
        ByBehavior = 3,
    };
}
namespace via::motion::JointExMultiRemapValue::ConeInputData {
    enum class ConeAxis : int64_t {
        X = 0,
        Y = 1,
        Z = 2,
        MinusX = 3,
        MinusY = 4,
        MinusZ = 5,
    };
}
namespace app::ropeway::weapon::SparkShotLCDController {
    enum class ChargeState : int64_t {
        Charging = 0,
        DisCharge = 1,
        Wait = 999,
    };
}
namespace app::ropeway::timeline::TimelineEventSettingTable::CameraSetting {
    enum class CurveMode : int64_t {
        LINEAR = 0,
        EASE_IN_ = 1,
        EASE_OUT = 2,
        EASE_INOUT = 3,
        CUSTOM = 4,
    };
}
namespace via::motion::IkSpineConformGround {
    enum class RAY_TYPE : int64_t {
        DEFAULT = 0,
        JOINT = 1,
    };
}
namespace app::ropeway::InputDefine::AnalogStick {
    enum class Kind : int64_t {
        None = 0,
        Left = 1,
        Right = 2,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class PlayerControlPhase : int64_t {
        FASTEST = -48,
        Condition = -47,
        Think = -46,
        Controller = -45,
        Weapon = -44,
        Twist = -43,
        IKFit = -42,
        LATEST = -41,
    };
}
namespace app::ropeway::camera {
    enum class CameraTwirlType : int64_t {
        CONSTANT = 0,
        ACCELARATE = 1,
        SWITCH = 2,
    };
}
namespace via::wwise {
    enum class PriorityMode : int64_t {
        Newest = 0,
        Oldest = 1,
    };
}
namespace app::ropeway::enemy::em3000::MotionPattern {
    enum class Standup : int64_t {
        FaceUpF = 0,
        FaceUpB = 1,
        FaceDown = 2,
    };
}
namespace app::ropeway::gui::NewInventorySlotBehavior {
    enum class DecideType : int64_t {
        NORMAL = 0,
        USE = 1,
        EQUIP = 2,
        EQUIP_OFF = 3,
        CUSTOM = 4,
        CUSTOM_OFF = 5,
        SHORTCUT = 6,
        COMBINE_CHOOSE = 7,
        COMBINE_DECIDE = 8,
        EXCHANGE = 9,
        GET_ITEM = 10,
        GO_DETAIL = 11,
        CHANGE_ID = 12,
        GO_PUZZLE = 13,
        OPEN_DIALOG = 14,
        PUTAWAY = 15,
        PICKUP = 16,
    };
}
namespace via::effect::detail {
    enum class ClipKeyType : int64_t {
        Bool = 0,
        Int32 = 1,
        Int64 = 2,
        UInt32 = 3,
        UInt64 = 4,
        Float = 5,
        Double = 6,
    };
}
namespace app::ropeway::InputUnit::SwayStick {
    enum class Event : int64_t {
        INVALID = 0,
        START = 1,
        PAUSE = 2,
        RESUME = 3,
        RESET = 4,
        STOP = 5,
        END = 6,
    };
}
namespace app::ropeway::weapon::shell::ShellBase{
    enum class BulletShellStatusInformation___ :  int64_t {
        NoSuspend = 0,
        Update = 1,
        LateUpdate = 2,
    };
}
namespace app::ropeway::enemy::common::fsmv2::condition::LastDamageDirection {
    enum class DirectionType : int64_t {
        ModeForward = 0,
        ModeSide = 1,
        ModeDir4 = 2,
        ModeDir8 = 3,
        ModeCross = 4,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::EmCommonMFsmAction_GM_Interact {
    enum class WindowInteractActionType : int64_t {
        BREAK = 0,
        LOCK_ONLY = 1,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class BodyPartsCategory : int64_t {
        Head = 0,
        Body = 1,
        Arms = 2,
        Legs = 3,
        Unique = 4,
    };
}
namespace app::ropeway::LocationStandbyController {
    enum class ExecFolderStatus : int64_t {
        None = 0,
        NoneToStandbyRoot = 1,
        NoneToStandbyWaitRegistLocationControl = 2,
        NoneToStandbyAttribute = 3,
        NoneToStandbyLoadLevel = 4,
        StandbyToNoneLoadLevel = 5,
        StandbyToNoneAttribute = 6,
        StandbyToNoneRoot = 7,
        Standby = 8,
        StandbyToActiveAttribute = 9,
        StandbyToActiveCamera = 10,
        StandbyToActiveLoadLevel = 11,
        ActiveToStandbyCamera = 12,
        ActiveToStandbyLoadLevel = 13,
        Active = 14,
    };
}
namespace via::render::ColorCorrectLinearParams {
    enum class LinearCorrector : int64_t {
        None = 0,
        Hue = 1,
        Chroma = 2,
        Brightness = 3,
        Sepia = 4,
        Scale = 5,
        NegaPosi = 6,
        GrayScale = 7,
        RedReplace = 8,
        GreenReplace = 9,
        BlueReplace = 10,
        Add = 11,
        Sub = 12,
        Max = 13,
    };
}
namespace via::timeline {
    enum class PropertyType : int64_t {
        Unknown = 0,
        Bool = 1,
        S8 = 2,
        U8 = 3,
        S16 = 4,
        U16 = 5,
        S32 = 6,
        U32 = 7,
        S64 = 8,
        U64 = 9,
        F32 = 10,
        F64 = 11,
        Str8 = 12,
        Str16 = 13,
        Enum = 14,
        Quaternion = 15,
        Array = 16,
        NativeArray = 17,
        Class = 18,
        NativeClass = 19,
        Struct = 20,
        Vec2 = 21,
        Vec3 = 22,
        Vec4 = 23,
        Color = 24,
        Range = 25,
        Float2 = 26,
        Float3 = 27,
        Float4 = 28,
        RangeI = 29,
        Point = 30,
        Size = 31,
        Asset = 32,
        Action = 33,
        Guid = 34,
        Uint2 = 35,
        Uint3 = 36,
        Uint4 = 37,
        Int2 = 38,
        Int3 = 39,
        Int4 = 40,
        OBB = 41,
        Mat4 = 42,
        Rect = 43,
        PathPoint3D = 44,
        Plane = 45,
        Sphere = 46,
        Capsule = 47,
        AABB = 48,
        Nullable = 49,
    };
}
namespace app::ropeway::gui::MenuStoryBehavior {
    enum class CameraState : int64_t {
        INVALID = 0,
        TO_SUB = 1,
        OUT_SUB = 2,
    };
}
namespace app::ropeway::camera::CameraShake::ShakeWorkInherited {
    enum class ParameterType : int64_t {
        TRANSLATION_X = 0,
        TRANSLATION_Y = 1,
        TRANSLATION_Z = 2,
        ROTATION_X = 3,
        ROTATION_Y = 4,
        ROTATION_Z = 5,
        MAX = 6,
    };
}
namespace via::navigation::AIMapEffector {
    enum class ShapeType : int64_t {
        ColliderBoundary = 0,
        AABB = 1,
        OBB = 2,
        Sphere = 3,
        MeshOutline = 4,
    };
}
namespace app::ropeway::OptionManager {
    enum class PresetKind : int64_t {
        Starting = 0,
        InitialSetting = 1,
        PresetSetting = 2,
    };
}
namespace via::effect::graph {
    enum class ErrorType : int64_t {
        NoERROR_ = 0,
        InvalidType = 1,
        InsufficientMemory = 2,
        InvalidOperation = 3,
    };
}
namespace app::ropeway::gui::FloorMapSt1BBehavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_8_408_0a = 1,
        Room_1_630_0a = 2,
        Room_1_630_0b = 3,
        Room_1_630_0c = 4,
        Room_1_631_0a = 5,
        Room_1_631_0b = 6,
        Room_1_632_0a = 7,
        Room_1_632_0b = 8,
        Room_1_632_0c = 9,
    };
}
namespace via {
    enum class SharePlayControllerMode : int64_t {
        Disable = 0xFFFFFFFF,
        Failed = -2,
        Invalid = -3,
        WatchingHostPlay = 0,
        PlayingAsHost = 1,
        PlayingWithHost = 2,
    };
}
namespace app::ropeway::IkHumanRetargetSolver::AdjustHipSolverData {
    enum class AdjustmentMode : int64_t {
        MUTABLE = 0,
        JOINT = 1,
        LEG_IK = 2,
    };
}
namespace app::ropeway::enemy::em3000 {
    enum class RequestSetID : int64_t {
        INVALID = 0xFFFFFFFF,
        PRESS_SURVIVOR = 0,
        PRESS_ENEMY = 1,
        PRESS_DEFAULT_ONLY = 2,
        DAMAGE = 3,
        SENSOR_TOUCH = 4,
        ATTACK_CLAW_R = 5,
        ATTACK_CLAW_L = 6,
        ATTACK_CLAW_TWO_R = 7,
        ATTACK_CLAW_TWO_L = 8,
        ATTACK_JUMP_CLAW_R = 9,
        ATTACK_JUMP_CLAW_L = 10,
        ATTACK_CLAW_FLASH_R = 11,
        ATTACK_CLAW_FLASH_L = 12,
        ATTACK_CLAW_TWO_FLASH_R = 13,
        ATTACK_CLAW_TWO_FLASH_L = 14,
        ATTACK_TONGUE = 15,
        ATTACK_HOLD = 16,
        SENSOR_WALL_STICK_ATTACK = 17,
        SENSOR_CEILLING_STICK_ATTACK = 18,
        SENSOR_PLAYER_CHECK = 19,
        SENSOR_GIMMICK = 20,
        DECAL_CLAW_R = 21,
        DECAL_CLAW_L = 22,
        DECAL_JUMP_CLAW_R = 23,
        DECAL_JUMP_CLAW_L = 24,
        DRAW_CONTROL = 25,
        AIM_TARGET = 26,
    };
}
namespace via::gui {
    enum class PageAlignment : int64_t {
        LeftTop = 0,
        LeftCenter = 4,
        LeftBottom = 8,
        CenterTop = 1,
        CenterCenter = 5,
        CenterBottom = 9,
        RightTop = 2,
        RightCenter = 6,
        RightBottom = 10,
    };
}
namespace app::ropeway::ComponentStopControl {
    enum class ControlLayer : int64_t {
        SELF = 0,
        EDITED_BY_SCENE = 1,
        GIMMICK_MANAGER_SUSPEND = 2,
        FSM_OSC = 3,
        FSM_DEACTIVE = 4,
        ENVIRONMENT_STANDBY_CONTROLLER = 5,
        GIMMICK_UTIL = 6,
        RESTAGE_EVENT_FINISHED = 7,
    };
}
namespace app::ropeway::timeline::TimelineDefine {
    enum class EventID : int64_t {
        CF001 = 10001,
        CF002 = 10002,
        CF003 = 10003,
        CF005 = 10005,
        CF006 = 10006,
        CF007 = 10007,
        CF008 = 10008,
        CF009 = 10009,
        CF010 = 10010,
        CF090 = 10090,
        CF092 = 10092,
        CF093 = 10093,
        CF094 = 10094,
        CF095 = 10095,
        CF100 = 10100,
        CF120 = 10120,
        CF130 = 10130,
        CF131 = 10131,
        CF132 = 10132,
        CF150 = 10150,
        CF151 = 10151,
        CF152 = 10152,
        CF153 = 10153,
        CF190 = 10190,
        CF220 = 10220,
        CF230 = 10230,
        CF240 = 10240,
        CF252 = 10252,
        CF290 = 10290,
        CF300 = 10300,
        CF320 = 10320,
        CF340 = 10340,
        CF350 = 10350,
        CF360 = 10360,
        CF380 = 10380,
        CF420 = 10420,
        CF470 = 10470,
        CF475 = 10475,
        CF480 = 10480,
        CF520 = 10520,
        CF535 = 10535,
        CF540 = 10540,
        CF559 = 10559,
        CF560 = 10560,
        CF590 = 10590,
        CF630 = 10630,
        CF632 = 10632,
        CF650 = 10650,
        CF670 = 10670,
        CF672 = 10672,
        CF674 = 10674,
        CF676 = 10676,
        CF677 = 10677,
        CF678 = 10678,
        CF693 = 10693,
        CF695 = 10695,
        CF705 = 10705,
        CF706 = 10706,
        CF707 = 10707,
        CF708 = 10708,
        CF720 = 10720,
        CF732 = 10732,
        CF770 = 10770,
        CF780 = 10780,
        CF790 = 10790,
        CF800 = 10800,
        CF810 = 10810,
        CF850 = 10850,
        CF860 = 10860,
        CF868 = 10868,
        CF870 = 10870,
        CF880 = 10880,
        CF892 = 10892,
        CF910 = 10910,
        CF912 = 10912,
        CF913 = 10913,
        CF914 = 10914,
        CF916 = 10916,
        CF930 = 10930,
        CF938 = 10938,
        EV000 = 20000,
        EV011 = 20011,
        EV020 = 20020,
        EV030 = 20030,
        EV040 = 20040,
        EV050 = 20050,
        EV051 = 20051,
        EV100 = 20100,
        EV110 = 20110,
        EV140 = 20140,
        EV220 = 20220,
        EV250 = 20250,
        EV251 = 20251,
        EV260 = 20260,
        EV330 = 20330,
        EV400 = 20400,
        EV410 = 20410,
        EV530 = 20530,
        EV580 = 20580,
        EV620 = 20620,
        EV680 = 20680,
        EV690 = 20690,
        EV700 = 20700,
        EV710 = 20710,
        EV730 = 20730,
        EV740 = 20740,
        EV750 = 20750,
        EV760 = 20760,
        EV890 = 20890,
        EV891 = 20891,
        EV900 = 20900,
        EV905 = 20905,
        EV980 = 20980,
        EV981 = 20981,
        EV982 = 20982,
        EV983 = 20983,
        EV984 = 20984,
        EV985 = 20985,
        EV986 = 20986,
        EV987 = 20987,
        EV990 = 20990,
        EV991 = 20991,
        EV992 = 20992,
        EV993 = 20993,
        Invalid = 0xFFFFFFFF,
    };
}
namespace via::hid::hmd::MorpheusTrackerState {
    enum class StatusCombination : int64_t {
        None = 0,
        Position = 4,
        Velocity = 8,
        Acceleration = 16,
        Orientation = 32,
        AngularVelocity = 64,
        AngularAcceleration = 128,
        AccelerometerPosition = 256,
        AccelerometerVelocity = 512,
        AccelerometerAcceleration = 1024,
        CameraPitchAngle = 2048,
        CameraRollAngle = 4096,
    };
}
namespace app::ropeway::gui::DefenseItemIconBehavior {
    enum class State : int64_t {
        NONE = 0,
        TO_ON = 1,
        DRAWING = 2,
        TO_OFF = 3,
    };
}
namespace via::storage::saveService {
    enum class SaveParcentCompleteStatus : int64_t {
        Start = 0,
        Serialize = 1,
        SizeCheck = 25,
        WriteFile = 50,
        TempUp = 75,
        End = 100,
    };
}
namespace app::ropeway::fsmv2::SetColliderValidation {
    enum class LayerType : int64_t {
        ALL = 1,
        MARKER = 2,
        TERRAIN_ = 4,
        EMHIT = 8,
        EMPASS = 16,
    };
}
namespace app::ropeway::enemy::em8500::Em8500Think {
    enum class EyeState : int64_t {
        Default = 0,
        Dead = 1,
    };
}
namespace app::ropeway::CartridgeController {
    enum class State : int64_t {
        INVALID = 0,
        MOVE = 1,
        STAY = 2,
        DEAD = 3,
    };
}
namespace app::ropeway::gimmick::option::InterludeSettings {
    enum class EndConditionType : int64_t {
        SceneAllLoaded = 0,
        CheckFlags = 1,
        RequesterDestroyed = 2,
        LModeEnd = 3,
    };
}
namespace via::network::wrangler {
    enum class EventLatency : int64_t {
        Undefined = 0,
        Normal = 1,
        RealTime = 2,
        ProviderDefault = 3,
    };
}
namespace app::ropeway::fsmv2::ConstActionTargetAction {
    enum class ConstTargetType : int64_t {
        OwnerObject = 0,
        ActionTargetObject = 1,
    };
}
namespace via::navigation::Navigation {
    enum class StopType : int64_t {
        Arrived = 0,
        Blocked = 1,
        CallStop = 2,
        AroundAttribute = 3,
        AroundPortal = 4,
        ERROR_ = 5,
        OnNode = 6,
    };
}
namespace app::ropeway::gui::GameOverFadeBehavior {
    enum class Type : int64_t {
        NONE = 0,
        SHORT = 1,
        NORMAL = 2,
        LONG = 3,
    };
}
namespace app::ropeway::SurvivorDefine::ActionOrder {
    enum class ActionCategoryBits : int64_t {
        IDLE = 1,
        MOVE = 2,
        JOG = 4,
        HOLD = 8,
    };
}
namespace app::ropeway::enemy::tracks::Em4400ColliderTrack {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Press = 0,
        Damage = 1,
        Sensor_Touch = 2,
        Sensor_Hearing = 3,
        Attack = 4,
    };
}
namespace app::ropeway::gimmick::option::EnemyConditionCheckSettings::EnemyConditionCheckParam {
    enum class AreaCheckType : int64_t {
        Same = 0,
        Near = 1,
        NoActive = 2,
    };
}
namespace via::effect::script {
    enum class WeaponID : int64_t {
        Hand = 0,
        HandAxe = 1,
        CircularSaw = 2,
        Knife = 3,
        Handgun = 4,
        Handgun_M19 = 5,
        Handgun_G17 = 6,
        Handgun_MPM = 7,
        Handgun_Albert = 8,
        ShotGun = 9,
        Shotgun_M37 = 10,
        Shotgun_M37S = 11,
        Shotgun_DB = 12,
        MachineGun = 13,
        Magnum = 14,
        GrenadeLauncher = 15,
        Burner = 16,
        Candle = 17,
        Glasses = 18,
        EvelynRadar = 19,
        LiquidBomb = 20,
        Remedy = 21,
        Stimulant = 22,
        Depressant = 23,
        KitchenKnife = 24,
        ChainSaw = 25,
        WoodChip = 26,
        HandLight = 27,
        ChainCutter = 28,
        ScrewDriver = 29,
        Shovel = 30,
        Lantern = 31,
        Roller = 32,
        Scissors = 33,
        Stick = 34,
        Num = 35,
        Etc = 36,
        wp1010 = 37,
        wp0000 = 38,
        wp0100 = 39,
        wp0200 = 40,
        wp0210 = 41,
        wp0300 = 42,
        wp0400 = 43,
        wp0500 = 44,
        wp0600 = 45,
        wp0700 = 46,
        wp0800 = 47,
        wp0900 = 48,
        wp1000 = 49,
        wp1100 = 50,
        wp1200 = 51,
        wp1300 = 52,
        wp1400 = 53,
        wp1500 = 54,
        wp1600 = 55,
        wp1700 = 56,
        wp1800 = 57,
        wp1900 = 58,
        wp2000 = 59,
        wp2100 = 60,
        wp2200 = 61,
        wp2300 = 62,
        wp2400 = 63,
        wp2500 = 64,
        wp2600 = 65,
        wp2700 = 66,
        wp2800 = 67,
        wp2900 = 68,
        wp3000 = 69,
        wp3010 = 70,
        wp3100 = 71,
        wp3200 = 72,
        wp3300 = 73,
        wp3400 = 74,
        wp3500 = 75,
        wp3600 = 76,
        wp3700 = 77,
        wp3800 = 78,
        wp3900 = 79,
        wp4000 = 80,
        wp4010 = 81,
        wp4020 = 82,
        wp4100 = 83,
        wp4110 = 84,
        wp4120 = 85,
        wp4130 = 86,
        wp4200 = 87,
        wp4300 = 88,
        wp4400 = 89,
        wp4410 = 90,
        wp4500 = 91,
        wp4510 = 92,
        wp4520 = 93,
        wp4600 = 94,
        wp4610 = 95,
        wp4700 = 96,
        wp4800 = 97,
        wp4900 = 98,
        wp5000 = 99,
        wp5100 = 100,
        wp5200 = 101,
        wp5300 = 102,
        wp5400 = 103,
        wp5500 = 104,
        wp5600 = 105,
        wp5700 = 106,
        wp5800 = 107,
        wp5900 = 108,
        wp6000 = 109,
        wp6100 = 110,
        wp6200 = 111,
        wp6300 = 112,
        wp6400 = 113,
        wp6500 = 114,
        wp6600 = 115,
        wp6700 = 116,
        wp6800 = 117,
        wp6900 = 118,
        wp7000 = 119,
        wp7100 = 120,
        wp7200 = 121,
        wp7300 = 122,
        wp7400 = 123,
        wp7500 = 124,
        wp7600 = 125,
        wp7700 = 126,
        wp7800 = 127,
        wp7900 = 128,
        wp8000 = 129,
        wp8100 = 130,
        wp8200 = 131,
        wp8300 = 132,
        wp8400 = 133,
        wp8500 = 134,
        wp8600 = 135,
        wp8700 = 136,
        wp8800 = 137,
        wp8900 = 138,
        wp9000 = 139,
        wp9100 = 140,
        wp9200 = 141,
        wp9300 = 142,
        wp9400 = 143,
        wp9500 = 144,
        wp9600 = 145,
        wp9700 = 146,
        wp9800 = 147,
        wp9900 = 148,
    };
}
namespace via::clr {
    enum class SignatureFlag : int64_t {
        Generic = 16,
        HasThis = 32,
        ExplicitThis = 64,
    };
}
namespace via::wwise::FreeArea {
    enum class FreeArea8to11 : int64_t {
        FreeArea8to11_None = 0xFFFFFFFF,
        FreeArea8to11_8 = 8,
        FreeArea8to11_9 = 9,
        FreeArea8to11_10 = 10,
        FreeArea8to11_11 = 11,
    };
}
namespace via::render::fluid {
    enum class TextureResourceType : int64_t {
        Element1 = 0,
        Element2 = 1,
        Element4 = 2,
        Max = 3,
    };
}
namespace via::hid::mouse {
    enum class ManipulatorClientTypeDev : int64_t {
        NULL_ = 0,
        WindowMessage = 3,
        GlobalParameter = 4,
        ToolDefault = 6,
    };
}
namespace via::render::layer::Overlay {
    enum class PriorityOrder : int64_t {
        GUICaptureSrcTex = 0,
        GUIRenderToRenderTarget = 1,
        GUIRenderToFrameBuffer = 2,
    };
}
namespace app::ropeway::enemy::em0000::RoleAction {
    enum class WindowIn : int64_t {
        BREAK_TO_IN_ = 0,
        BREAKIN_ = 1,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class Dead : int64_t {
        DEAD_F_00 = 0,
        DEAD_F_01 = 1,
        DEAD_F_02 = 2,
        DEAD_B_00 = 3,
        DEAD_B_01 = 4,
        DEAD_B_02 = 5,
        DEAD_B_03 = 6,
        DEAD_LOW_F_00 = 7,
        DEAD_LOW_F_01 = 8,
        DEAD_LOW_F_02 = 9,
        DEAD_LOW_F_03 = 10,
        DEAD_LOW_B_00 = 11,
        DEAD_LOW_B_01 = 12,
        DEAD_LOW_B_02 = 13,
        DEAD_LOW_B_03 = 14,
        DEAD_FACEDOWN_00 = 15,
        DEAD_FACEDOWN_01 = 16,
        DEAD_FACEDOWN_02 = 17,
        DEAD_FACEUP_00 = 18,
        DEAD_FACEUP_01 = 19,
        DEAD_FACEUP_02 = 20,
        DEAD_FACEUP_03 = 21,
        DEAD_FACEUP_04 = 22,
        DEAD_INERTIA_00 = 23,
        DEAD_INERTIA_01 = 24,
        DEAD_INERTIA_02 = 25,
        DEAD_LOST_L_00 = 26,
        DEAD_LOST_L_01 = 27,
        DEAD_LOST_R_00 = 28,
        DEAD_LOST_R_01 = 29,
        DEAD_STOMPED_FACEDOWN_00 = 30,
        DEAD_STOMPED_FACEUP_00 = 31,
        DEADPOSE_FACEDOWN_00 = 32,
        DEADPOSE_FACEDOWN_01 = 33,
        DEADPOSE_FACEDOWN_02 = 34,
        DEADPOSE_FACEUP_00 = 35,
        DEADPOSE_FACEUP_01 = 36,
        DEADPOSE_FACEUP_02 = 37,
        DEADPOSE_FACEUP_03 = 38,
        WALLDOWN_00 = 39,
        WALLDOWN_01 = 40,
        WALLUP_00 = 41,
        WALLUP_01 = 42,
        WALLUP_02 = 43,
        DESK_00 = 44,
        DESK_01 = 45,
    };
}
namespace app::ropeway::NpcDefine {
    enum class LookAtBattleModeType : int64_t {
        DISABLE = 0,
        LOOK_AT_TARGET_HEAD = 1,
        LOOK_AT_AIM_POINT = 2,
        LOOK_AT_NOTICE_POINT = 3,
    };
}
namespace app::ropeway::gamemastering::Map {
    enum class ID : int64_t {
        Invalid = 0,
        st0_101_0 = 1,
        st0_102_0 = 2,
        st1_101_0 = 3,
        st1_102_0 = 4,
        st1_103_0 = 5,
        st1_104_0 = 6,
        st1_105_0 = 7,
        st1_106_0 = 8,
        st1_107_0 = 9,
        st1_108_0 = 10,
        st1_109_0 = 11,
        st1_110_0 = 12,
        st1_111_0 = 13,
        st1_201_0 = 14,
        st1_202_0 = 15,
        st1_203_0 = 16,
        st1_204_0 = 17,
        st1_205_0 = 18,
        st1_206_0 = 19,
        st1_207_0 = 20,
        st1_208_0 = 21,
        st1_209_0 = 22,
        st1_210_0 = 23,
        st1_211_0 = 24,
        st2_101_0 = 25,
        st2_102_0 = 26,
        st2_103_0 = 27,
        st2_201_0 = 28,
        st2_202_0 = 29,
        st2_203_0 = 30,
        st2_204_0 = 31,
        st2_205_0 = 32,
        st2_206_0 = 33,
        st2_207_0 = 34,
        st2_208_0 = 35,
        st2_209_0 = 36,
        st2_210_0 = 37,
        st2_211_0 = 38,
        st2_212_0 = 39,
        st2_213_0 = 40,
        st2_214_0 = 41,
        st2_215_0 = 42,
        st2_216_0 = 43,
        st2_301_0 = 44,
        st2_302_0 = 45,
        st2_303_0 = 46,
        st2_304_0 = 47,
        st2_305_0 = 48,
        st2_306_0 = 49,
        st2_307_0 = 50,
        st2_308_0 = 51,
        st2_309_0 = 52,
        st2_401_0 = 53,
        st2_401_1 = 54,
        st2_402_0 = 55,
        st2_403_0 = 56,
        st2_404_0 = 57,
        st2_405_0 = 58,
        st2_406_0 = 59,
        st2_407_0 = 60,
        st2_408_0 = 61,
        st2_501_0 = 62,
        st2_502_0 = 63,
        st2_502_0b = 64,
        st2_502_0c = 65,
        st2_503_0 = 66,
        st2_504_0 = 67,
        st2_505_0 = 68,
        st2_506_0 = 69,
        st2_507_0 = 70,
        st2_508_0 = 71,
        st2_601_0 = 72,
        st2_602_0 = 73,
        st2_603_0 = 74,
        st2_604_0 = 75,
        st2_605_0 = 76,
        st2_606_0 = 77,
        st2_607_0 = 78,
        st2_608_0 = 79,
        st3_101_0 = 80,
        st3_102_0 = 81,
        st3_103_0 = 82,
        st3_104_0 = 83,
        st3_105_0 = 84,
        st3_106_0 = 85,
        st3_107_0 = 86,
        st3_108_0 = 87,
        st3_109_0 = 88,
        st3_110_0 = 89,
        st3_111_0 = 90,
        st3_112_0 = 91,
        st3_113_0 = 92,
        st3_114_0 = 93,
        st3_201_0 = 94,
        st3_202_0 = 95,
        st3_203_0 = 96,
        st3_204_0 = 97,
        st3_205_0 = 98,
        st3_206_0 = 99,
        st3_207_0 = 100,
        st3_208_0 = 101,
        st3_209_0 = 102,
        st3_210_0 = 103,
        st3_301_0 = 104,
        st3_302_0 = 105,
        st3_303_0 = 106,
        st3_304_0 = 107,
        st3_305_0 = 108,
        st3_306_0 = 109,
        st3_307_0 = 110,
        st3_308_0 = 111,
        st4_101_0 = 112,
        st4_102_0 = 113,
        st4_103_0 = 114,
        st4_104_0 = 115,
        st4_105_0 = 116,
        st4_106_0 = 117,
        st4_207_0 = 118,
        st4_208_0 = 119,
        st4_209_0 = 120,
        st4_210_0 = 121,
        st4_211_0 = 122,
        st4_212_0 = 123,
        st4_213_0 = 124,
        st4_214_0 = 125,
        st5_101_0 = 126,
        st5_102_0 = 127,
        st5_103_0 = 128,
        st5_104_0 = 129,
        st5_105_0 = 130,
        st5_106_0 = 131,
        st5_107_0 = 132,
        st5_201_0 = 133,
        st5_202_0 = 134,
        st5_203_0 = 135,
        st5_204_0 = 136,
        st5_205_0 = 137,
        st5_206_0 = 138,
        st5_207_0 = 139,
        st5_301_0 = 140,
        st5_302_0 = 141,
        st5_303_0 = 142,
        st6_101_0 = 143,
        st6_102_0 = 144,
        st6_103_0 = 145,
        st6_104_0 = 146,
        st6_105_0 = 147,
        st6_106_0 = 148,
        st6_107_0 = 149,
        st6_201_0 = 150,
        st6_202_0 = 151,
        st6_203_0 = 152,
        st6_204_0 = 153,
        st6_205_0 = 154,
        st6_206_0 = 155,
        st6_207_0 = 156,
        st6_208_0 = 157,
        st6_209_0 = 158,
        st6_210_0 = 159,
        st6_211_0 = 160,
        st6_212_0 = 161,
        st7_101_0 = 162,
        st7_102_0 = 163,
        st7_103_0 = 164,
        st7_104_0 = 165,
        st7_105_0 = 166,
        st7_106_0 = 167,
        st7_107_0 = 168,
        st7_108_0 = 169,
        st7_109_0 = 170,
        st7_110_0 = 171,
        st7_111_0 = 172,
        st7_112_0 = 173,
        st8_101_0 = 174,
        st8_102_0 = 175,
        st8_103_0 = 176,
        st8_104_0 = 177,
        st8_105_0 = 178,
        st8_106_0 = 179,
        st8_107_0 = 180,
        st8_108_0 = 181,
        st8_109_0 = 182,
        st8_110_0 = 183,
        st8_111_0 = 184,
        st8_112_0 = 185,
        st8_113_0 = 186,
        st8_114_0 = 187,
        st8_115_0 = 188,
        st8_201_0 = 189,
        st8_202_0 = 190,
        st8_203_0 = 191,
        st8_301_0 = 192,
        st8_302_0 = 193,
        st8_303_0 = 194,
        st8_304_0 = 195,
        st8_305_0 = 196,
        st8_306_0 = 197,
        st8_307_0 = 198,
        st8_308_0 = 199,
        st8_309_0 = 200,
        st8_310_0 = 201,
        st8_311_0 = 202,
        st8_401_0 = 203,
        st8_402_0 = 204,
        st8_403_0 = 205,
        st8_404_0 = 206,
        st8_405_0 = 207,
        st8_406_0 = 208,
        st8_407_0 = 209,
        st9_101_0 = 210,
        st9_102_0 = 211,
        st9_103_0 = 212,
        st9_201_0 = 213,
        st9_202_0 = 214,
        st9_203_0 = 215,
        st9_301_0 = 216,
        st1_301_0 = 217,
        st1_401_0 = 218,
        st4_201_0 = 219,
        st4_202_0 = 220,
        st4_203_0 = 221,
        st4_204_0 = 222,
        st4_205_0 = 223,
        st4_206_0 = 224,
        st4_215_0 = 225,
        st4_216_0 = 226,
        st4_301_0 = 227,
        st4_302_0 = 228,
        st4_303_0 = 229,
        st4_304_0 = 230,
        st4_305_0 = 231,
        st4_306_0 = 232,
        st4_307_0 = 233,
        st4_308_0 = 234,
        st4_309_0 = 235,
        st4_310_0 = 236,
        st4_311_0 = 237,
        st4_312_0 = 238,
        st4_313_0 = 239,
        st4_314_0 = 240,
        st4_401_0 = 241,
        st4_401_1 = 242,
        st4_402_0 = 243,
        st4_403_0 = 244,
        st4_404_0 = 245,
        st4_405_0 = 246,
        st4_406_0 = 247,
        st4_407_0 = 248,
        st4_408_0 = 249,
        st4_409_0 = 250,
        st4_410_0 = 251,
        st4_411_0 = 252,
        st4_412_0 = 253,
        st4_501_0 = 254,
        st4_502_0 = 255,
        st4_503_0 = 256,
        st4_504_0 = 257,
        st4_505_0 = 258,
        st4_506_0 = 259,
        st4_507_0 = 260,
        st4_508_0 = 261,
        st4_601_0 = 262,
        st4_602_0 = 263,
        st4_603_0 = 264,
        st4_604_0 = 265,
        st4_605_0 = 266,
        st4_606_0 = 267,
        st4_607_0 = 268,
        st4_608_0 = 269,
        st4_609_0 = 270,
        st4_610_0 = 271,
        st4_701_0 = 272,
        st4_702_0 = 273,
        st4_703_0 = 274,
        st4_704_0 = 275,
        st4_705_0 = 276,
        st4_708_0 = 277,
        st4_709_0 = 278,
        st4_710_0 = 279,
        st4_711_0 = 280,
        st4_712_0 = 281,
        st4_714_0 = 282,
        st1_501_0 = 283,
        st1_502_0 = 284,
        st1_503_0 = 285,
        st1_504_0 = 286,
        st1_505_0 = 287,
        st1_506_0 = 288,
        st4_650_0 = 289,
        st3_401_0 = 290,
        st3_402_0 = 291,
        st3_403_0 = 292,
        st3_404_0 = 293,
        st3_405_0 = 294,
        st3_406_0 = 295,
        st3_407_0 = 296,
        st3_408_0 = 297,
        st3_409_0 = 298,
        st3_410_0 = 299,
        st8_501_0 = 300,
        st8_601_0 = 301,
        st8_602_0 = 302,
        st8_603_0 = 303,
        st8_604_0 = 304,
        st8_605_0 = 305,
        st8_606_0 = 306,
        st8_607_0 = 307,
        st8_608_0 = 308,
        st8_609_0 = 309,
        st3_600_0 = 310,
        st3_601_0 = 311,
        st3_602_0 = 312,
        st3_603_0 = 313,
        st3_610_0 = 314,
        st3_611_0 = 315,
        st3_612_0 = 316,
        st3_613_0 = 317,
        st3_614_0 = 318,
        st3_615_0 = 319,
        st3_616_0 = 320,
        st3_617_0 = 321,
        st3_620_0 = 322,
        st3_621_0 = 323,
        st3_622_0 = 324,
        st3_623_0 = 325,
        st3_624_0 = 326,
        st3_625_0 = 327,
        st3_626_0 = 328,
        st3_627_0 = 329,
        st3_630_0 = 330,
        st3_631_0 = 331,
        st3_632_0 = 332,
        st3_633_0 = 333,
        st3_634_0 = 334,
        st3_635_0 = 335,
        st3_636_0 = 336,
        st3_637_0 = 337,
        st3_638_0 = 338,
        st3_640_0 = 339,
        st3_641_0 = 340,
        st3_642_0 = 341,
        st3_643_0 = 342,
        st3_644_0 = 343,
        st3_650_0 = 344,
        st3_651_0 = 345,
        st3_652_0 = 346,
        st1_411_0 = 347,
        st4_715_0 = 348,
        st4_716_0 = 349,
        st4_717_0 = 350,
        st8_408_0 = 351,
        st4_750_0 = 352,
        st4_751_0 = 353,
        st4_752_0 = 354,
        st4_753_0 = 355,
        st4_754_0 = 356,
        st8_409_0 = 357,
        st8_410_0 = 358,
        st1_601_0 = 359,
        st1_602_0 = 360,
        st1_603_0 = 361,
        st1_604_0 = 362,
        st1_605_0 = 363,
        st1_606_0 = 364,
        st1_607_0 = 365,
        st1_608_0 = 366,
        st1_609_0 = 367,
        st8_650_0 = 368,
        st8_411_0 = 369,
        st5_111_0 = 370,
        st5_121_0 = 371,
        st5_131_0 = 372,
        st5_211_0 = 373,
        st5_221_0 = 374,
        st0_103_0 = 375,
        st8_660_0 = 376,
        st1_620_0 = 377,
        st1_621_0 = 378,
        st1_622_0 = 379,
        st1_623_0 = 380,
        st1_610_0 = 381,
        st1_611_0 = 382,
        st1_612_0 = 383,
        st1_613_0 = 384,
        st1_614_0 = 385,
        st5_112_0 = 386,
        st5_110_0 = 387,
        st5_113_0 = 388,
        st5_114_0 = 389,
        st5_115_0 = 390,
        st5_307_0 = 391,
        st5_308_0 = 392,
        st5_309_0 = 393,
        st5_310_0 = 394,
        st5_401_0 = 395,
        st5_402_0 = 396,
        st5_403_0 = 397,
        st5_404_0 = 398,
        st5_405_0 = 399,
        st5_406_0 = 400,
        st5_407_0 = 401,
        st5_408_0 = 402,
        st4_800_0 = 403,
        st1_615_0 = 404,
        st4_755_0 = 405,
        st1_630_0 = 406,
        st1_631_0 = 407,
        st1_632_0 = 408,
        st4_713_0 = 409,
        st5_122_0 = 410,
        st5_123_0 = 411,
        st5_124_0 = 412,
        st5_125_0 = 413,
        st5_126_0 = 414,
        st5_127_0 = 415,
        st5_128_0 = 416,
        st5_129_0 = 417,
        st5_132_0 = 418,
        st5_133_0 = 419,
        st5_134_0 = 420,
        st5_212_0 = 421,
        st5_222_0 = 422,
        st5_223_0 = 423,
        st8_651_0 = 424,
        st8_610_0 = 425,
        st8_611_0 = 426,
        st8_612_0 = 427,
        st8_613_0 = 428,
        st3_604_0 = 429,
        st3_605_0 = 430,
        st4_217_0 = 431,
        st5_122_1 = 432,
        st5_122_2 = 433,
        st8_614_0 = 434,
        MAP_NUM = 435,
    };
}
namespace app::ropeway::ChoreographPlayer::ChoreographMovePosition::Position {
    enum class PosType : int64_t {
        ONSET = 0,
        WORLD = 1,
        RELATIVE_ = 2,
    };
}
namespace app::ropeway::enemy::em3000::fsmv2::action::SetGroundState {
    enum class EXEC_TYPE : int64_t {
        START = 0,
        END = 1,
        FRAME = 2,
        SEQUENCE = 3,
    };
}
namespace via::hid {
    enum class TouchScreenAction : int64_t {
        Move = 0,
        Down = 1,
        Up = 2,
        Cancel = 3,
    };
}
namespace via::render {
    enum class RegisteredMeshType : int64_t {
        Static = 0,
        Dynamic = 1,
    };
}
namespace app::ropeway::enemy::em9000::Em9000Think {
    enum class HandItemType : int64_t {
        HandLight = 0,
        FireAxe = 1,
        KeyRing = 2,
    };
}
namespace via::render {
    enum class RegisterComponent : int64_t {
        Unknown = 0,
        Uint32 = 1,
        Sint32 = 2,
        Float32 = 3,
    };
}
namespace app::ropeway::enemy::em6000::fsmv2::action::Em6000FsmAction_Hide {
    enum class SettingAttr : int64_t {
        Invincible = 0,
        NoDamage = 1,
        SensorAllOff = 2,
        SensorAllOn = 3,
        OnlyTouchSensorOn = 4,
        AimTragetOff = 5,
        HateClear = 6,
        EndPLFind = 7,
        EscapeWarp = 8,
        RoleWarp = 9,
    };
}
namespace via::areamap::test::RegionDensity {
    enum class Condition : int64_t {
        Densest = 0,
        Sparsest = 1,
    };
}
namespace app::ropeway::gui::RogueInventoryBehavior {
    enum class BackState : int64_t {
        NoMove = 0,
        Wait = 1,
    };
}
namespace via::render::Stamp {
    enum class StampChannelMask : int64_t {
        All = 15,
        R = 1,
        G = 2,
        B = 4,
        A = 8,
        RG = 3,
        RB = 5,
        RA = 9,
        GB = 6,
        GA = 10,
        BA = 12,
        RGB = 7,
        RBA = 13,
        RGA = 11,
        GBA = 14,
        None = 0,
    };
}
namespace System::Diagnostics {
    enum class DebuggerBrowsableState : int64_t {
        Never = 0,
        Collapsed = 2,
        RootHidden = 3,
    };
}
namespace via::network::wrangler {
    enum class FieldType : int64_t {
        UnicodeString = 0,
        Int8 = 1,
        UInt8 = 2,
        Int16 = 3,
        UInt16 = 4,
        Int32 = 5,
        UInt32 = 6,
        Int64 = 7,
        UInt64 = 8,
        Float = 9,
        Double = 10,
        Boolean = 11,
        Binary = 12,
        GUID = 13,
        Pointer = 14,
        FILETIME = 15,
        SYSTEMTIME = 16,
        CountedUnicodeString = 17,
        IPv4 = 18,
        IPv6 = 19,
    };
}
namespace via::render::VolumeDecal {
    enum class WholeBlendMode : int64_t {
        None = 0,
        BaseColor = 1,
        Normal = 2,
    };
}
namespace via {
    enum class AccountPickerTarget : int64_t {
        Auto = 0,
        UnmanagedDevice = 1,
        LastInputDevice = 2,
    };
}
namespace app::ropeway::effect::script::EffectRecordSave {
    enum class EmitTypes : int64_t {
        Stamp = 0,
        Decal = 1,
    };
}
namespace app::ropeway::CollisionDefine {
    enum class AttackAttribute : int64_t {
        INFINITE_HIT = 1,
        OWNER_HIT = 2,
        NOT_KILL = 4,
        THROUGH_TERRAIN_ = 8,
    };
}
namespace via::puppet::RemoteGameObject {
    enum class Priority : int64_t {
        None = 0,
        Unreliable = 1,
        Reliable = 2,
        Both = 3,
        SyncImmediate = 4,
    };
}
namespace via::autoplay::action::AutoAttack {
    enum class Unit : int64_t {
        Second = 0,
        Frame = 1,
    };
}
namespace via::os::PackReader {
    enum class Source : int64_t {
        Unknown = 0,
        Package = 1,
        DLC = 2,
        Any = 3,
    };
}
namespace via::eq::Parser {
    enum class Error : int64_t {
        None = 0,
        Syntax = 1,
        InvalidUnaryOperator = 2,
        InvalidNumber = 3,
        InvalidVariable = 4,
        InvalidEquation = 5,
        MemoryInsufficient = 6,
        Critical = 7,
    };
}
namespace app::ropeway::EnemyAttackInitiativeManager::AttackEnemyInfo {
    enum class STATE : int64_t {
        INVALID = 0,
        HOLD = 1,
        ATTACK = 2,
        RANGE_OUT = 3,
    };
}
namespace app::ropeway::gui::RogueInGameShopBehavior {
    enum class SaleWeaponPartsID : int64_t {
        sm71_901 = 48,
        sm71_902 = 49,
        sm71_903 = 50,
        sm71_904 = 51,
        sm71_905 = 52,
        sm71_906 = 53,
        sm71_907 = 54,
        sm71_908 = 55,
        sm71_909 = 56,
        sm71_911 = 58,
        sm71_913 = 60,
        sm71_914 = 61,
        sm71_915 = 62,
        sm71_917 = 64,
        sm71_918 = 65,
        sm71_919 = 66,
    };
}
namespace via::effect::script::StampController {
    enum class Type : int64_t {
        Blood = 0,
        Scar = 1,
        Water = 2,
        Other = 3,
        Num = 4,
    };
}
namespace app::ropeway::posteffect::setting::FilterSettingRoot {
    enum class ApplyType : int64_t {
        AtTopLayer = 0,
        Always = 1,
    };
}
namespace app::ropeway::WwiseStateManager {
    enum class BackgroundExecuteType : int64_t {
        FALSE_ = 0,
        TRUE_ = 1,
    };
}
namespace via::wwise::WwiseGlobalUserVariablesValue {
    enum class TypeKind : int64_t {
        Unknown = 0,
        Boolean = 1,
        Int32 = 2,
        Uint32 = 3,
        Single = 4,
    };
}
namespace app::ropeway::survivor::player::PlayerFacialController {
    enum class SituationMode : int64_t {
        NONE = 0xFFFFFFFF,
        DEAD = 0,
        GRAPPLE = 1,
        NUM = 2,
    };
}
namespace via::timeline {
    enum class ExecuteGroup : int64_t {
        ExecuteGroup_00 = 0,
        ExecuteGroup_01 = 1,
        ExecuteGroup_02 = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickTerrainStep {
    enum class ActionType : int64_t {
        UP = 0,
        DOWN = 1,
    };
}
namespace via::areamap::test::RegionArea {
    enum class Condition : int64_t {
        Biggest = 0,
        Smallest = 1,
    };
}
namespace app::ropeway::enemy::em5000 {
    enum class ColliderSkipID : int64_t {
        UL_Body00 = 0,
        UL_Body01 = 1,
        UL_Body02 = 2,
        UR_Body03 = 3,
        UR_Body04 = 4,
        UR_Body05 = 5,
        DL_Body06 = 6,
        DL_Body07 = 7,
        DL_Body08 = 8,
        DR_Body09 = 9,
        DR_Body10 = 10,
        DR_Body11 = 11,
        UL_LeftArm00 = 12,
        UL_LeftArm01 = 13,
        UL_LeftArm02 = 14,
        UR_RightArm00 = 15,
        UR_RightArm01 = 16,
        UR_RightArm02 = 17,
        DL_LeftLeg00 = 18,
        DL_LeftLeg01 = 19,
        DL_LeftLeg02 = 20,
        DR_RightLeg00 = 21,
        DR_RightLeg01 = 22,
        DR_RightLeg02 = 23,
    };
}
namespace app::ropeway::enemy::em3000::MotionPattern {
    enum class Claw : int64_t {
        ClawL = 0,
        ClawR = 1,
        Offset = 2,
    };
}
namespace via::motion::ExtraJoint {
    enum class AttrFlags : int64_t {
        DisableEmptyInterpolation = 1,
    };
}
namespace via {
    enum class Access : int64_t {
        Private = 0,
        Public = 1,
    };
}
namespace via::motion::detail {
    enum class ChainCollisionType : int64_t {
        Self = 0,
        Model = 1,
        Collider = 2,
        VGround = 3,
    };
}
namespace app::ropeway::gui::FloatIconParam {
    enum class FloatIconPositionFormat : int64_t {
        MESH_BOUNDARY_CENTER = 0,
        MESH_BOUNDARY_CENTER_TOP = 1,
        FOCUS_POSITION = 2,
        TRANSFORM = 3,
        FILE = 4,
        MANUAL = 5,
        CUSTOM = 6,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class KindID : int64_t {
        em0000 = 0,
        em0100 = 1,
        em0200 = 2,
        em3000 = 3,
        em4000 = 4,
        em4100 = 5,
        em4400 = 6,
        em5000 = 7,
        em6000 = 8,
        em6100 = 9,
        em6200 = 10,
        em6300 = 11,
        em7000 = 12,
        em7100 = 13,
        em7110 = 14,
        em7200 = 15,
        em7300 = 16,
        em7400 = 17,
        em9000 = 18,
        em8000 = 19,
        em8100 = 20,
        em8200 = 21,
        em8300 = 22,
        em8400 = 23,
        em8500 = 24,
        em9999 = 25,
        MAX = 26,
        Invalid = 0xFFFFFFFF,
    };
}
namespace app::ropeway::enemy::em7200::Em7200Think {
    enum class WallJumpStateEnum : int64_t {
        START = 0,
        GOTO_WALL = 1,
        WALL_KICK = 2,
        GOTO_TARGET = 3,
        GOTO_FRONT_POS = 4,
        END = 5,
    };
}
namespace via::clr {
    enum class VMObjType : int64_t {
        NULL_ = 0,
        Object = 1,
        Array = 2,
        String = 3,
        Delegate = 4,
        ValType = 5,
    };
}
namespace app::ropeway::gimmick::action::GimmickControl {
    enum class SuspendType : int64_t {
        SOLO_DEACTIVE_MAP = 2,
        PAIR_NO_BODY = 3,
    };
}
namespace via::clr {
    enum class MethodFlag : int64_t {
        AccessibilityMask = 7,
        PrivateScope = 0,
        Private = 1,
        FamANDAssem = 2,
        Assembly = 3,
        Family = 4,
        FamORAssem = 5,
        Public = 6,
        Static = 16,
        Final = 32,
        Virtual = 64,
        HideBySig = 128,
        NewSlot = 256,
        Abstract = 1024,
        SpecialName = 2048,
        PinvokeImpl = 8192,
        UnmanagedExp = 8,
        RTSpecialName = 4096,
        NoILAsmKeyword = 16384,
        ReqsecObj = 32768,
    };
}
namespace app::ropeway::gamemastering::RogueRecordManager {
    enum class C_CourceType : int64_t {
        Short = 0,
        Long = 1,
        MAX = 2,
    };
}
namespace via::wwise::FreeArea {
    enum class FreeArea12to15 : int64_t {
        FreeArea12to15_None = 0xFFFFFFFF,
        FreeArea12to15_12 = 12,
        FreeArea12to15_13 = 13,
        FreeArea12to15_14 = 14,
        FreeArea12to15_15 = 15,
    };
}
namespace app::ropeway::gimmick::action::GimmickCustomJackControl::TriggerAdditionalForEnemy {
    enum class TriggerType : int64_t {
        NONE = 0,
        DAMAGE = 1,
    };
}
namespace via::effect::script::EffectDecal {
    enum class DecalFollowEnum : int64_t {
        NotFollow = 0,
        FollowEffect = 1,
    };
}
namespace app::ropeway::enemy::em3000::MotionPattern {
    enum class DamageWall : int64_t {
        FromF = 0,
        FromB = 1,
        FromL = 2,
        FromR = 3,
        FromFR = 4,
        FromFL = 5,
        FromBR = 6,
        FromBL = 7,
    };
}
namespace via::relib::effect::behavior::ReLibMarkStampController {
    enum class Type : int64_t {
        Blood = 0,
        Scar = 1,
        Water = 2,
        Other = 3,
        Num = 4,
    };
}
namespace app::ropeway::rogue::RogueDefine {
    enum class SoundState : int64_t {
        INVALID = 0,
        TITLE = 1,
        BATTLE = 2,
        RESULT = 3,
    };
}
namespace app::ropeway::Em6100Think {
    enum class CLASS : int64_t {
        Normal = 0,
        King = 1,
        NUM = 2,
    };
}
namespace app::ropeway::gimmick::action::GimmickCommonBreakableControl {
    enum class HitCategory : int64_t {
        ALL = 0,
        TOPL_ALL = 1,
        TOEM_ALL = 2,
        BreakablePillar = 3,
    };
}
namespace via::navigation {
    enum class WarningStatus : int64_t {
        Enable = 0,
        EnableContinuous = 1,
        Disable = 2,
    };
}
namespace app::ropeway::enemy::em0000::fsmv2::condition::Em0000MFsmCondition_BrokenStatus {
    enum class ReactionPattern : int64_t {
        STANDING_2_LIMPING_L = 0,
        STANDING_2_LIMPING_R = 1,
        STANDING_2_KNEELING_L = 2,
        STANDING_2_KNEELING_R = 3,
        STANDING_2_TRAILING_L = 4,
        STANDING_2_TRAILING_R = 5,
        KNEELING_L_2_TRAILING_L = 6,
        KNEELING_R_2_TRAILING_R = 7,
        LIMPING_L_2_CREEPING = 8,
        LIMPING_R_2_CREEPING = 9,
        KNEELING_L_2_CREEPING = 10,
        KNEELING_R_2_CREEPING = 11,
        TRAILING_L_2_CREEPING = 12,
        TRAILING_R_2_CREEPING = 13,
        CREEPING_LOST_L_2_CREEPING = 14,
        CREEPING_LOST_R_2_CREEPING = 15,
        CREEPING_LOST_L_FACEUP_2_CREEPING = 16,
        CREEPING_LOST_R_FACEUP_2_CREEPING = 17,
    };
}
namespace app::ropeway::enemy::em6300::Em6300Think {
    enum class ENABLE_ACTION : int64_t {
        Anythig = 0,
        NailAtk = 1,
        NailAtk2 = 2,
        Thrust = 3,
        BackAtk = 4,
        Garipper = 5,
        PowerDunk = 6,
    };
}
namespace app::ropeway::network::service::AuthorizationServiceControllerForSteam {
    enum class Phase : int64_t {
        None = 0,
        Requested = 1,
        ReqAuthCode = 2,
    };
}
namespace via::effect::detail {
    enum class RotationOrder : int64_t {
        XYZ = 0,
        XZY = 1,
        YXZ = 2,
        YZX = 3,
        ZXY = 4,
        ZYX = 5,
    };
}
namespace app::ropeway::navigation::DirectLinkController {
    enum class ElementKind : int64_t {
        A = 0,
        B = 1,
        Invalid = 2,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class Parts : int64_t {
        None = 0,
        Body = 1,
        Face = 2,
        Hair = 4,
        Other = 8,
        All = 15,
    };
}
namespace app::ropeway::AssetPackagePair {
    enum class ClassType : int64_t {
        UNSET = 0xFFFFFFFF,
        PLAYER = 0,
        ENEMY = 1,
        MAX = 2,
    };
}
namespace app::ropeway::gimmick::option::MovePositionSettings {
    enum class RotationType : int64_t {
        ONSET_ROT = 0,
        TARGET_ROT = 1,
        TARGET_OBJ = 2,
        TURN_TO_TARGET_POS = 3,
        TURN_TO_TARGET_OBJ = 4,
    };
}
namespace via::render {
    enum class DistortionType : int64_t {
        None = 0,
        Left = 1,
        Right = 2,
    };
}
namespace app::ropeway::gui::MenuScenarioBehavior {
    enum class DecidedResult : int64_t {
        LEON = 0,
        CLAIRE = 1,
        MAX = 2,
        CANCEL = 3,
    };
}
namespace via::motion::MotionAppendData {
    enum class PropertyType : int64_t {
        Unknown = 0,
        Enum = 1,
        Boolean = 2,
        Int8 = 3,
        Uint8 = 4,
        Int16 = 5,
        Uint16 = 6,
        Int32 = 7,
        Uint32 = 8,
        Int64 = 9,
        Uint64 = 10,
        Single = 11,
        Double = 12,
        C8 = 13,
        C16 = 14,
        Char = 14,
        String = 15,
        Struct = 16,
        Class = 17,
    };
}
namespace app::ropeway::enemy::em6300::MotionPattern {
    enum class DashStart : int64_t {
        Normal = 0,
        FromWalk = 1,
    };
}
namespace app::ropeway::gui::BaseInfo {
    enum class Mode : int64_t {
        NoDisp = 0,
        Opened = 1,
        Entried = 2,
    };
}
namespace via::render::VolumeDecal {
    enum class Priority : int64_t {
        Highest = 0,
        Higher = 1,
        High = 2,
        Middle = 3,
        Default = 4,
        Low = 5,
        Lower = 6,
        Lowest = 7,
        Max = 8,
    };
}
namespace via::effect::script::TinyTimer {
    enum class Mode : int64_t {
        None = 0,
        Up = 1,
        Down = 2,
    };
}
namespace via::gui {
    enum class GlyphAtlasSize : int64_t {
        Size512x512 = 0,
        Size1024x512 = 1,
        Size1024x1024 = 2,
        Size2048x1024 = 3,
        Size2048x2048 = 4,
        Size4096x2048 = 5,
        Size4096x4096 = 6,
    };
}
namespace via::hid::camera {
    enum class PlayStationCameraChannel : int64_t {
        Left = 0,
        Right = 1,
        Max = 2,
    };
}
namespace via::render::PrimitiveMesh::Attribute {
    enum class PipelineIndex : int64_t {
        Transparent = 0,
        TwoSideTransparent = 1,
        DistortionDepth = 2,
        GBuffer = 3,
        Shadow = 4,
        DebugOverdraw = 5,
        Max = 6,
    };
}
namespace System {
    enum class MidpointRounding : int64_t {
        ToEven = 0,
        AwayFromZero = 1,
    };
}
namespace app::ropeway::enemy::em7300::tracks::Em7300WallUpCheck {
    enum class CheckLRType : int64_t {
        WALL_L = 0,
        WALL_R = 1,
        WALL_CENTER = 2,
        WALL_DASH = 3,
    };
}
namespace app::ropeway::enemy::em3000::MotionPattern {
    enum class StickClaw : int64_t {
        WallL = 0,
        WallR = 1,
        CeilingL = 2,
        CeilingR = 3,
    };
}
namespace app::ropeway::gui::GeneralGuideBehavior {
    enum class ButtonType : int64_t {
        NoDisp = 0,
        Disp_OkBack = 1,
        Disp_SkipBack = 2,
        Disp_SkipBack_Keyboard = 3,
        Disp_Ok = 4,
        Disp_Back = 5,
        Disp_Next = 6,
        Disp_Close = 7,
        Disp_InvMove = 8,
        Disp_InvOkCancel = 9,
        Disp_File = 10,
        Disp_FileTuto = 11,
        Disp_Map = 12,
        Disp_MapOne = 13,
        Disp_FileNext = 14,
        Disp_FileClose = 15,
        Disp_FileNextClose = 16,
        Disp_ItemSelect = 17,
        Disp_Detail = 18,
        Disp_ItemBox = 19,
        Disp_ItemBoxCancel = 20,
        Disp_ItemBoxNum = 21,
        Disp_GmkSelect = 22,
        Disp_GmkRotate = 23,
        Disp_GmkSelectRotate = 24,
        Disp_GmkStonePuzzle = 25,
        Disp_GmkSwitchboardPuzzle = 26,
        Disp_GmkMedicinePuzzle = 27,
        Disp_PzlPortableSafe = 28,
        Disp_PzlOscilloscope = 29,
        Disp_Pause = 30,
        Disp_Skip = 31,
        Disp_Skip_Keyboard = 32,
        Disp_ResetOkBack = 33,
        Disp_ResetOk = 34,
        Disp_EasyOk = 35,
        Disp_FigureDetail = 36,
        Disp_ScrollOk = 37,
        Disp_ScrollOkClose = 38,
        Disp_ScrollOkBack = 39,
        Disp_ScrollBack = 40,
        Disp_AccountOk = 41,
        Disp_OkBackSelectable = 42,
        Disp_ResetBack = 43,
        Disp_OkResetDeleteBack = 44,
        Disp_CancelDelete = 45,
        Disp_TitleLGuide = 46,
        Disp_ScrollResetBack = 47,
        Disp_AccessoryGuide = 48,
        Disp_GmkSelect_LR = 49,
    };
}
namespace app::ropeway::SurvivorCostumeEventConvertUserData::ConvertParam {
    enum class ConvertMode : int64_t {
        Both = 0,
        OneSide = 1,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class PositioningPhase : int64_t {
        FASTEST = -66,
        Press = -65,
        GroundFixer = -64,
        TransformInterpolator = -63,
        StayAreaController = -62,
        LATEST = -61,
    };
}
namespace via::render::ToneMapping {
    enum class TemporalAA : int64_t {
        Legacy = 0,
        Manual = 1,
        Weak = 2,
        Mild = 3,
        Strong = 4,
        Disable = 5,
    };
}
namespace via::render {
    enum class DrawMode : int64_t {
        Default = 1,
        ShadowCast = 2,
        Envmap = 4,
        Voxelize = 8,
        PreCompute = 16,
    };
}
namespace via::render::layer {
    enum class GBufferSegment : int64_t {
        ZIgnorePrepassSolid = 0,
        ZIgnorePrepassTwoSide = 1,
        ZIgnorePrepassTwoSideAlphaTest = 2,
        ZIgnorePrepassAlphaTest = 3,
        ZPrepassSolid = 5,
        ZPrepassTwoSide = 6,
        ZPrepassTwoSideAlphaTest = 7,
        ZPrepassAlphaTest = 8,
        ZPrepassVfx = 9,
        Solid = 10,
        TwoSide = 11,
        DefaultZPrepass = 12,
        TwoSideAlphaTest = 13,
        AlphaTest = 14,
        EmissiveMask = 16,
        EmissiveSolid = 26,
        EmissiveTwoSide = 27,
        EmissiveTwoSideAlphaTest = 29,
        EmissiveAlphaTest = 30,
        MeshDecal = 31,
        DepthWrite = 32,
        DepthWrittenSolid = 33,
        ViewScaling = 34,
        ViewScalingSolid = 35,
        DecalPrepare = 48,
        DisplacementPrepare = 50,
        Displacement = 51,
        PreDecalBlend = 52,
        PreDecalDepthOnly = 53,
        Decal = 54,
        PostDecalBlend = 55,
        PostDecalDepthOnly = 56,
        DepthWritePostDecal = 58,
        DepthWrittenSolidPostDecal = 59,
        ViewScalingPostDecal = 60,
        ViewScalingSolidPostDecal = 61,
    };
}
namespace app::ropeway::NpcDefine {
    enum class LookAtMode : int64_t {
        DISABLE = 0,
        RESIDENT = 1,
        BATTLE = 2,
    };
}
namespace via::render {
    enum class RenderOutputID : int64_t {
        Primary = 1,
        Secondary = 2,
        Tertiary = 4,
        Quateary = 8,
        All = 15,
    };
}
namespace via::effect::script::EffectTimeline {
    enum class EndType : int64_t {
        PlayEnd = 0,
        Stop = 1,
        Resoter = 2,
        End = 3,
    };
}
namespace via::effect::script::EffectCommonDefine {
    enum class EffectActionOnParentDisappear : int64_t {
        None = 0,
        Finish = 1,
        Kill = 2,
        Unparent = 3,
    };
}
namespace app::Collision::HitManager {
    enum class Direction : int64_t {
        Front = 0,
        Back = 1,
        Left = 2,
        Right = 3,
        FrontLeft = 4,
        FrontRight = 5,
        BackLeft = 6,
        BackRight = 7,
    };
}
namespace via::motion {
    enum class MotionStateFlag : int64_t {
        None = 0,
        EndFrame = 1,
        NextEndFrame = 2,
        LoopHead = 4,
        LoopTail = 8,
        BlockEnd = 16,
        NextBlockEnd = 32,
        Setup = 128,
    };
}
namespace app::ropeway::gimmick::action::GimmickDoorBase {
    enum class DoorType : int64_t {
        SINGLE = 0,
        DOUBLE = 1,
        MAX = 2,
    };
}
namespace app::ropeway::gimmick::action::TriggerKey {
    enum class PushType : int64_t {
        DOWN = 0,
        HOLD = 1,
        LONG = 2,
    };
}
namespace app::ropeway::gui::MainTitleBehavior {
    enum class Type : int64_t {
        MAIN_TITLE = 0,
        RECORD = 1,
        CONCEPTART_LIST = 2,
        CONCEPTART = 3,
        FIGURE_LIST = 4,
        FIGURE = 5,
        MOVIE_LIST = 6,
        MOVIE = 7,
        COSTUME_LIST = 8,
        COSTUME = 9,
        ADA = 10,
        THE_4TH = 11,
        TOFU = 12,
        RECORD_ROGUE = 13,
        CAMERA_MODE = 14,
        PLAYER_SELECT = 15,
        CLEAR_STATE = 16,
        ROGUE = 17,
        CREDIT = 18,
    };
}
namespace via::render {
    enum class SparseShadowTreeReductionPixel : int64_t {
        SparseShadowTreeReductionPixel_None = 0,
        SparseShadowTreeReductionPixel_2px = 2,
        SparseShadowTreeReductionPixel_4px = 4,
        SparseShadowTreeReductionPixel_8px = 8,
        SparseShadowTreeReductionPixel_16px = 16,
        SparseShadowTreeReductionPixel_32px = 32,
        SparseShadowTreeReductionPixel_64px = 64,
        SparseShadowTreeReductionPixel_128px = 128,
    };
}
namespace app::ropeway::LookAt {
    enum class Index : int64_t {
        _LookAtJoint0 = 0,
        _LookAtJoint1 = 1,
        _LookAtJoint2 = 2,
        _LookAtJoint3 = 3,
        _LookAtJoint4 = 4,
        Head = 5,
        L_Eye = 6,
        R_Eye = 7,
    };
}
namespace app::Collision::CollisionSystem::AsyncCastHandleBase {
    enum class CastType : int64_t {
        Invalid = 0,
        Ray = 1,
        Sphere = 2,
    };
}
namespace app::ropeway::weapon::controller::ShellGeneratorControllerBase{
    enum class SparkShellGeneratorControlUserData :  int64_t {
        None = 0,
        Init = 1,
        Operation = 2,
        Destroy = 3,
        Invalid = 4,
    };
}
namespace via {
    enum class TimeFormat : int64_t {
        H12 = 0,
        H24 = 1,
        Max = 2,
        Unknown = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0200_MONTAGE : int64_t {
        ID000 = 0,
        ID001 = 1,
        ID002 = 2,
        ID003 = 3,
        ID004 = 4,
        ID005 = 5,
        ID006 = 6,
        ID007 = 7,
        ID008 = 8,
        ID200 = 9,
        ID201 = 10,
        ID202 = 11,
        ID203 = 12,
        ID800 = 13,
        ID801 = 14,
        ID802 = 15,
        ID803 = 16,
        ID804 = 17,
        ID900 = 18,
        ID999 = 19,
    };
}
namespace app::ropeway::enemy::em6300::MotionPattern {
    enum class DamageDash : int64_t {
        FromFL = 0,
        FromFR = 1,
        FromL = 2,
        FromR = 3,
    };
}
namespace via::fsm::Core {
    enum class WorkAttribute : int64_t {
        WorkAttributeSelectBeforeAction = 0,
        WorkAttributeBitNum = 32,
    };
}
namespace app::ropeway::gamemastering::GameMaster {
    enum class MachineDetailType : int64_t {
        INVALID = 0,
        PS4Base = 1,
        PS4NEO = 2,
        XB1 = 3,
        XB1X = 4,
    };
}
namespace app::ropeway::enemy::common::fsmv2::condition::LastDamageDirection {
    enum class FourDir : int64_t {
        Front = 0,
        Back = 1,
        Left = 2,
        Right = 3,
    };
}
namespace app::ropeway::enemy::em3000::fsmv2::action::Em3000FsmAction_TopboardSearch {
    enum class ConstMode : int64_t {
        ROOT = 0,
        NULL_OFFSET = 1,
    };
}
namespace app::ropeway::gui::OptionBehavior {
    enum class ListMode : int64_t {
        ROOT = 0,
        CONTROLS = 1,
        CONTROLS_BUTTON_TYPE = 2,
        CAMERA = 3,
        DISPLAY = 4,
        DISPLAY_BRIGHTNESS = 5,
        DISPLAY_SS_SETTING = 6,
        AUDIO = 7,
        LANGUAGE = 8,
        PC = 9,
        RE_NET = 10,
        RE_NET_DETAIL = 11,
        TO_DEFAULT = 12,
        WAIT_SAVE = 13,
        PC_PRESET = 14,
        TO_PRIVACY_POLICY = 15,
        Invalid = 16,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class JointType : int64_t {
        INVALID = 0,
        MALE = 1,
        FEMALE = 2,
        CHILD = 3,
    };
}
namespace via::motion::IkJacobian {
    enum class MomentumModeType : int64_t {
        Zero = 0,
        Animation = 1,
        TargetPos = 2,
    };
}
namespace app::ropeway::level::LoadGimmickSpecCatalog::GimmickFolderSpec {
    enum class PathBaseType : int64_t {
        Absolute = 0,
        EnvironmentRelative = 1,
    };
}
namespace app::ropeway::fsmv2::PlayTimelineComponent {
    enum class TargetType : int64_t {
        SELF = 0,
        CHILDREN = 1,
        KEY = 2,
        TARGET_OBJ = 3,
    };
}
namespace via::effect::detail {
    enum class GpuBlendType : int64_t {
        AlphaBlend = 0,
        Physical = 1,
        AddContrast = 2,
        EdgeBlend = 3,
        Multiply = 4,
    };
}
namespace app::ropeway::survivor::fsmv2::action::SurvivorRecoilAction {
    enum class Operation : int64_t {
        Increase = 0,
        Reset = 1,
    };
}
namespace app::ropeway::gamemastering::GlobalUserDataManager {
    enum class BoolResult : int64_t {
        INVALID = 0,
        FALSE_ = 1,
        TRUE_ = 2,
    };
}
namespace app::ropeway::NpcDefine {
    enum class NPCType : int64_t {
        INVALID = 0xFFFFFFFF,
        PL2000 = 2000,
        PL3000 = 3000,
    };
}
namespace via::effect::detail {
    enum class MaterialParameterType : int64_t {
        None = 0,
        Float = 1,
        Range = 2,
        Texture = 3,
    };
}
namespace app::ropeway::gui::GimmickNumberLockHothouseGuiBehavior2 {
    enum class ButtonType : int64_t {
        Key0 = 0,
        Key1 = 1,
        Key2 = 2,
        Key3 = 3,
        Key4 = 4,
        Key5 = 5,
        Key6 = 6,
        Key7 = 7,
        Key8 = 8,
        Key9 = 9,
        KeyBack = 10,
        KeyEnter = 11,
        MAX = 12,
        NONE = 12,
    };
}
namespace via::render::LightVolumeDecal {
    enum class BaseAlphaMapType : int64_t {
        BaseAlpha = 0,
        Msk1ch = 1,
    };
}
namespace app::ropeway::camera {
    enum class CameraLockType : int64_t {
        FREE = 0,
        PITCH = 1,
        YAW = 2,
        BOTH = 3,
    };
}
namespace app::ropeway::WaitCondition {
    enum class WaitType : int64_t {
        TIME = 1,
        KEY = 2,
        FLAG = 4,
        EXTERTNAL = 8,
    };
}
namespace app::ropeway::grapple {
    enum class GrapplePriority : int64_t {
        Priority_00 = 0,
        Priority_01 = 1,
        Priority_02 = 2,
        Priority_03 = 3,
        Priority_04 = 4,
        Priority_05 = 5,
        Priority_06 = 6,
        Priority_07 = 7,
        Invalid = 8,
    };
}
namespace app::ropeway::enemy::savedata::Em0000SaveDataBase {
    enum class BodyPartsIndex : int64_t {
        HEAD = 0,
        BODY = 1,
        ARM_L = 2,
        ARM_R = 3,
        LEG_L = 4,
        LEG_R = 5,
        __ARRAY_SIZE__ = 6,
    };
}
namespace app::ropeway::fsmv2::player::SetCharacterControllerRadius {
    enum class RadiusType : int64_t {
        INVALID = 0,
        DEFAULT = 1,
        HOLD = 2,
        HOLDED = 3,
        DEAD = 4,
    };
}
namespace via::effect::script::EffectManager {
    enum class PriorityType : int64_t {
        LastPriority = 0,
        FirstPriority = 1,
    };
}
namespace via::navigation::QueryObject {
    enum class WorkAttribute : int64_t {
        MapChanged = 0,
        WorkAttributeNum = 1,
    };
}
namespace app::ropeway::fsmv2::JackStateEndAction {
    enum class EndCheckMethodType : int64_t {
        MotionEnd = 0,
        MotionEndOrSequence = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickCircuitPuzzle {
    enum class ColorType : int64_t {
        NONE = 0,
        RED = 1,
        GREEN = 2,
        BLUE = 4,
        CYAN = 6,
        MAGENTA = 5,
        YELLOW = 3,
        WHITE = 7,
    };
}
namespace via::network {
    enum class ServiceType : int64_t {
        None = 0,
        Lamm = 1,
        Steam = 2,
        WeGame = 3,
        Psn = 4,
        Live = 5,
        Live_UWP = 6,
        PiaLocal = 7,
        PiaInet = 8,
        Max = 9,
    };
}
namespace via::effect::graph {
    enum class ContainerType : int64_t {
        Unknown = 0,
        Emiter = 1,
        Action = 2,
        Field = 3,
    };
}
namespace app::ropeway::InputDefine {
    enum class Kind : int64_t {
        NONE = 0,
        MOVE = 1,
        WATCH = 2,
        WALK = 4,
        JOG0 = 8,
        JOG1 = 16,
        QUICK_TURN = 32,
        HOLD = 64,
        SUPPORT_HOLD = 128,
        ATTACK = 256,
        RELOAD = 512,
        RESET_CAMERA = 1024,
        QUICK_TURN_EX = 2048,
        DEFENSE = 4096,
        ACTION = 8192,
        ITEM = 16384,
        GIMMICK_EX = 32768,
        PRESS_START = 65536,
        INVENTORY = 131072,
        CHANGE_BULLET = 262144,
        MINIMAP = 524288,
        PAUSE = 1048576,
        SKIP_EVENT = 2097152,
        DECIDE = 4194304,
        CANCEL = 8388608,
        UI_UP = 16777216,
        UI_DOWN = 33554432,
        UI_LEFT = 67108864,
        UI_RIGHT = 134217728,
        UI_SHIFT_LEFT = 268435456,
        UI_SHIFT_RIGHT = 536870912,
        UI_SHIFT_LEFT_2 = 1073741824,
        UI_SHIFT_RIGHT_2 = 2147483648,
        UI_EXCHANGE = 4294967296,
        UI_RESET = 8589934592,
        SHORTCUT_UP = 17179869184,
        SHORTCUT_DOWN = 34359738368,
        SHORTCUT_LEFT = 68719476736,
        SHORTCUT_RIGHT = 137438953472,
        UI_L_STICK = 274877906944,
        UI_R_STICK = 549755813888,
        DIALOG_DECIDE = 1099511627776,
        DIALOG_CANCEL = 2199023255552,
        UI_PAGE_UP = 4398046511104,
        UI_PAGE_DOWN = 8796093022208,
        UI_SCALE_UP = 17592186044416,
        UI_SCALE_DOWN = 35184372088832,
        UI_MAP_UP = 70368744177664,
        UI_MAP_DOWN = 140737488355328,
        UI_SORT = 281474976710656,
        UI_DISP = 562949953421312,
        FLASHLIGHT = 1125899906842624
    };
}
namespace via::gui::detail {
    enum class ControlPointH : int64_t {
        Left = 0,
        Center = 1,
        Right = 2,
    };
}
namespace via::wwise::WwiseFloatEnumConverterElement {
    enum class FloatEnum : int64_t {
        FloatEnum_0 = 0,
        FloatEnum_1 = 1,
        FloatEnum_2 = 2,
        FloatEnum_3 = 3,
        FloatEnum_4 = 4,
        FloatEnum_5 = 5,
        FloatEnum_6 = 6,
        FloatEnum_7 = 7,
        FloatEnum_8 = 8,
        FloatEnum_9 = 9,
        FloatEnum_Max = 10,
    };
}
namespace app::ropeway {
    enum class TwoFootLockBitFlag : int64_t {
        LEFT = 1,
        RIGHT = 2,
    };
}
namespace via::render {
    enum class RtvDimension : int64_t {
        Unknown = 0,
        Buffer = 1,
        Texture1d = 2,
        Texture1darray = 3,
        Texture2d = 4,
        Texture2darray = 5,
        Texture2dms = 6,
        Texture2dmsarray = 7,
        Texture3d = 8,
    };
}
namespace via::timeline {
    enum class PauseGroup : int64_t {
        PauseGroup_00 = 0,
        PauseGroup_01 = 1,
        PauseGroup_02 = 2,
        PauseGroup_03 = 3,
        PauseGroup_04 = 4,
        PauseGroup_05 = 5,
        PauseGroup_06 = 6,
        PauseGroup_07 = 7,
        PauseGroup_08 = 8,
        PauseGroup_09 = 9,
    };
}
namespace via::render {
    enum class TargetFlag : int64_t {
        None = 0,
        NeedIDBuffer = 1,
        Num = 2,
    };
}
namespace via::effect::detail {
    enum class AxisType : int64_t {
        PositiveX = 0,
        PositiveY = 1,
        PositiveZ = 2,
        NegativeX = 3,
        NegativeY = 4,
        NegativeZ = 5,
    };
}
namespace via::render {
    enum class ClearType : int64_t {
        RTV = 0,
        UAVUint = 1,
        UAVFloat = 2,
        DSV = 3,
        DELAY = 128,
        DELAYMASK = 127,
        RTV_DELAY = 128,
        UAVUint_DELAY = 129,
        UAVFloat_DELAY = 130,
        DSV_DELAY = 131,
    };
}
namespace via::motion::JointExBsplineConstraint {
    enum class Axis : int64_t {
        X = 0,
        Y = 1,
        Z = 2,
        NX = 3,
        NY = 4,
        NZ = 5,
    };
}
namespace via::hid::HIDEntry {
    enum class EnterButton : int64_t {
        RRight = 1,
        RDown = 2,
    };
}
namespace via::dynamics::RagdollConstraint::Initializer {
    enum class PositionType : int64_t {
        World = 0,
        Local = 1,
    };
}
namespace app::ropeway::InputSystem::CommandPart {
    enum class Result : int64_t {
        FAIL = 0,
        SUCCESS = 1,
        OPERATION = 2,
    };
}
namespace app::ropeway::gamemastering::RogueStorageManager {
    enum class StorageType : int64_t {
        INVALID = 0xFFFFFFFF,
        ITEMBOX = 0,
        RUCKSACK = 1,
    };
}
namespace app::ropeway::weapon::ElectrodeController {
    enum class StateKind : int64_t {
        Invalid = 0,
        Init = 1,
        Operation = 2,
        Torn = 3,
        End = 4,
    };
}
namespace app::ropeway::enemy::em5000::RoleMotionID {
    enum class Idle : int64_t {
        IDLE = 4,
    };
}
namespace app::ropeway::gamemastering::PurposeManager {
    enum class PurposeState : int64_t {
        INVALID = 0,
        WAIT = 1,
        DISP = 2,
        END = 3,
        ALREADY = 4,
        MAX = 5,
    };
}
namespace via::motion::IkMultipleDamageAction {
    enum class Calculation : int64_t {
        Default = 0,
        AddDirection = 1,
    };
}
namespace via::effect::gpgpu::DeformedMesh::MeshCsResource {
    enum class SkinningType : int64_t {
        None = 0,
        Weight4 = 1,
        Weight8 = 2,
        Max = 3,
    };
}
namespace via::network::wrangler {
    enum class ProviderPriority : int64_t {
        Undefined = 0,
        Normal = 1,
        Critical = 2,
    };
}
namespace via::collision::detail {
    enum class CalcToiRet : int64_t {
        NoHit = 0,
        HitNoMove = 1,
        HitCcd = 2,
    };
}
namespace via::motion::IkSpineConformGround {
    enum class COG_MANAGE_TYPE : int64_t {
        INTERNAL = 0,
        EXTERNAL = 1,
    };
}
namespace app::ropeway::weapon::FlamethrowerTachoController {
    enum class TachoStates : int64_t {
        RevUp = 0,
        RevDown = 1,
    };
}
namespace via::navigation::NavigationManager {
    enum class NavigationUpdateTiming : int64_t {
        Prev = 1,
        Late = 2,
    };
}
namespace via::navigation {
    enum class UpdateTiming : int64_t {
        Default = 0,
        Prev = 1,
        Late = 2,
    };
}
namespace via::dynamics::Hinge::Initializer {
    enum class PositionType : int64_t {
        World = 0,
        Local = 1,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class DrawType : int64_t {
        TOTAL_BODY = 0,
        ONLY_BODY = 1,
    };
}
namespace app::ropeway::enemy::em5000::BehaviorTreeAction {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_MOVE = 1,
        ACT_ATTENTION = 2,
        ATK_HOLD = 2000,
    };
}
namespace app::ropeway::gui::CostumeBehavior {
    enum class MainIndex : int64_t {
        LEON = 0,
        CLAIRE = 1,
        DUMMY = 2,
        MAX = 3,
        INVALID = 4,
    };
}
namespace via::clr {
    enum class SignatureModFlag : int64_t {
        Ptr = 1,
        ByRef = 2,
        ValueType = 4,
        Class = 8,
        Var = 16,
        Array = 32,
        GenericInst = 64,
        FNPTr = 128,
        SzArray = 256,
        MVar = 512,
        CModReqd = 1024,
        CModOpt = 2048,
        Stencil = 4096,
        Pinned = 8192,
    };
}
namespace via::effect::detail {
    enum class Rotate90 : int64_t {
        None = 0,
        Rotate = 1,
        RandomRotate = 2,
    };
}
namespace via::render::layer::Transparent {
    enum class SegmentOrder : int64_t {
        PreTransparent = 0,
        Transparent = 1,
        PreparePostTransparent = 2,
        PostTransparent = 3,
        ZPrepass = 4,
        RoughTransparent = 5,
        Distortion = 6,
        PostDistortion = 7,
        Haze = 8,
        PostNoDepth = 9,
        Lensflare = 10,
        Primitive2D = 11,
        TransparentOverlay = 12,
        BlendBuffer = 13,
    };
}
namespace app::ropeway::enemy::em3000::MotionPattern {
    enum class Step : int64_t {
        Back = 0,
        Left = 1,
        Right = 2,
    };
}
namespace app::ropeway::timeline::ProgramInfo {
    enum class ConditionType : int64_t {
        SCENARIO = 0,
        GLOVAL_VARIABLES = 1,
    };
}
namespace via::gui {
    enum class SamplerType : int64_t {
        PointWrap = 0,
        PointClamp = 1,
        BilinearWrap = 4,
        BilinearClamp = 5,
    };
}
namespace app::ropeway::enemy::HateTargetInfo {
    enum class FindSourceType : int64_t {
        Unknown = 0,
        Spawn = 1,
        Damage = 2,
        Sound = 3,
        Touch = 4,
        Sight = 5,
        FsmControl = 6,
        Territory = 7,
    };
}
namespace app::ropeway::gamemastering::RogueBaseAreaFlow {
    enum class Result : int64_t {
        Move = 0,
        EndToExtraMenu = 1,
        EndToInGame = 2,
    };
}
namespace app::ropeway::enemy::em7100 {
    enum class LongMurderType : int64_t {
        LongMurderType_Normal = 0,
        LongMurderType_R90 = 1,
        LongMurderType_Combo = 2,
    };
}
namespace via::render {
    enum class TextureStreamingType : int64_t {
        None = 0,
        Streaming = 1,
        HighMap = 2,
    };
}
namespace app::ropeway::gimmick::option::GimmickCameraSettings {
    enum class SpotLightType : int64_t {
        NONE = 0,
        FORCE = 1,
        REMOTE = 2,
    };
}
namespace app::ropeway::timeline::TimelineDefine {
    enum class CF : int64_t {
        CF001 = 10001,
        CF002 = 10002,
        CF003 = 10003,
        CF005 = 10005,
        CF006 = 10006,
        CF007 = 10007,
        CF008 = 10008,
        CF009 = 10009,
        CF010 = 10010,
        CF090 = 10090,
        CF092 = 10092,
        CF093 = 10093,
        CF094 = 10094,
        CF095 = 10095,
        CF100 = 10100,
        CF120 = 10120,
        CF130 = 10130,
        CF131 = 10131,
        CF132 = 10132,
        CF150 = 10150,
        CF151 = 10151,
        CF152 = 10152,
        CF153 = 10153,
        CF190 = 10190,
        CF220 = 10220,
        CF230 = 10230,
        CF240 = 10240,
        CF252 = 10252,
        CF290 = 10290,
        CF300 = 10300,
        CF320 = 10320,
        CF340 = 10340,
        CF350 = 10350,
        CF360 = 10360,
        CF380 = 10380,
        CF420 = 10420,
        CF470 = 10470,
        CF475 = 10475,
        CF480 = 10480,
        CF520 = 10520,
        CF535 = 10535,
        CF540 = 10540,
        CF559 = 10559,
        CF560 = 10560,
        CF590 = 10590,
        CF630 = 10630,
        CF632 = 10632,
        CF650 = 10650,
        CF670 = 10670,
        CF672 = 10672,
        CF674 = 10674,
        CF676 = 10676,
        CF677 = 10677,
        CF678 = 10678,
        CF693 = 10693,
        CF695 = 10695,
        CF705 = 10705,
        CF706 = 10706,
        CF707 = 10707,
        CF708 = 10708,
        CF720 = 10720,
        CF732 = 10732,
        CF770 = 10770,
        CF780 = 10780,
        CF790 = 10790,
        CF800 = 10800,
        CF810 = 10810,
        CF850 = 10850,
        CF860 = 10860,
        CF868 = 10868,
        CF870 = 10870,
        CF880 = 10880,
        CF892 = 10892,
        CF910 = 10910,
        CF912 = 10912,
        CF913 = 10913,
        CF914 = 10914,
        CF916 = 10916,
        CF930 = 10930,
        CF938 = 10938,
        Invalid = 0xFFFFFFFF,
    };
}
namespace app::ropeway::enemy::common::tracks::EnemySetGroundStateTrack {
    enum class STATE : int64_t {
        INVALID = 0,
        JUMP = 1,
        GROUND = 2,
    };
}
namespace app::ropeway::gui::GimmickHotHouseCtrlPanelGuiBehavior {
    enum class IconType : int64_t {
        POWER = 0,
        LADDER = 1,
    };
}
namespace via::hid::hmd::Morpheus {
    enum class VrModeStatusCheckTiming : int64_t {
        VrTrackerStarted = 0,
        VrVideoModeEnabled = 1,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class ACTION_CATEGORY : int64_t {
        ACTION = 0,
        GIMMICK = 1000,
        ATTACK = 2000,
        DAMAGE = 3000,
        SUPPORT = 4000,
        DIE = 5000,
        SET = 6000,
        DEMO = 7000,
    };
}
namespace app::ropeway::navigation::DirectLinkLadder {
    enum class LadderKind : int64_t {
        Upper = 0,
        Lower = 1,
        Invalid = 2,
    };
}
namespace app::ropeway::enemy::em6000 {
    enum class ColliderSkipID : int64_t {
        Normal = 2,
        Floating = 4,
        Ambush = 8,
        Swim = 32,
        NotSwim = 64,
        Down = 128,
        NotDown = 256,
        PusFull = 1024,
        PusBroken = 2048,
        BodySensorAll = 14,
    };
}
namespace via::wwise {
    enum class StopAllTargetType : int64_t {
        StopAllType_GameObject = 0,
        StopAllType_EntireComponent = 1,
        StopAllType_Global = 2,
    };
}
namespace app::ropeway::survivor::jack::SurvivorJackActionDefine {
    enum class StateKind : int64_t {
        Idle = 0,
        Interp = 1,
        WaitStartLoop = 2,
        Start = 3,
        Loop = 4,
        SecondStart = 5,
        SecondLoop = 6,
        End = 7,
        Terminate = 8,
        JackEnd = 9,
        Other = 10,
    };
}
namespace app::ropeway::enemy::userdata::Em0000AttackUserData {
    enum class AttackPatternType : int64_t {
        HoldFromWalk = 0,
        HoldFromCreep = 1,
        HoldPushDown = 2,
        HoldLightly = 3,
        HoldMouseFromWalk = 4,
        HoldOnBox = 5,
        Inertia = 6,
        Inertia_far = 7,
    };
}
namespace via::effect::script {
    enum class GroupFlag : int64_t {
        Player = 1,
        Enemy = 2,
        Item = 8,
        Weapon = 4,
        Prop = 16,
        Etc = 32,
        Vfx = 64,
        Camera = 128,
        ActionPoint = 256,
        EventChara = 512,
    };
}
namespace app::ropeway {
    enum class NoticeMode : int64_t {
        Normal = 0,
        Additional = 1,
        Hide = 2,
    };
}
namespace via::gui::detail {
    enum class SceneInfoAttribute : int64_t {
        GUICameraOnly = 0,
        PrimaryOnly = 1,
        Both = 2,
        Max = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickSkewLift {
    enum class Routine : int64_t {
        INIT = 0,
        DOWN_START = 1,
        DOWN_LOOP = 2,
        DOWN_END = 3,
        RISE_START = 4,
        RISE_LOOP = 5,
        RISE_END = 6,
    };
}
namespace app::ropeway::fsmv2::CharacterInterpolateTiltRequestFsmAction::Axis {
    enum class AxisType : int64_t {
        Manual = 0,
        WorldX = 1,
        WorldY = 2,
        WorldZ = 3,
        ObjectX = 4,
        ObjectY = 5,
        ObjectZ = 6,
    };
}
namespace via::Window {
    enum class Message : int64_t {
        Create = 0,
        Close = 1,
        Redraw = 2,
        Focus = 3,
        FocusWindow = 4,
        Activate = 5,
        Resize = 6,
        Notify = 7,
        MouseMove = 8,
        MouseWheel = 9,
        MouseDown = 10,
        MouseUp = 11,
        MouseDblClk = 12,
        WinIniChange = 13,
        TimeChange = 14,
        KeyDown = 15,
        KeyUp = 16,
        DeviceChange = 17,
        Input = 18,
        InputDeviceChange = 19,
        MouseDelta = 20,
        Max = 21,
    };
}
namespace via {
    enum class AccountPickerState : int64_t {
        NotSupported = 0,
        Idle = 1,
        Running = 2,
    };
}
namespace app::ropeway::RTTHolder {
    enum class RTTType : int64_t {
        WRIST_TAG_READER = 0,
    };
}
namespace via::network::Protocol {
    enum class SendOption : int64_t {
        None = 0,
        Unreliable = 0,
        Reliable = 1,
        ReliableBuffer = 3,
        FastCallback = 4,
        CheckAbsent = 8,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class EM0000_MONTAGE_PARTS_PANTS : int64_t {
        NONE = 0,
        PANTS00 = 1,
        PANTS01 = 2,
        PANTS01_00_00 = 3,
        PANTS01_00_01 = 4,
        PANTS02 = 5,
        PANTS03 = 6,
        PANTS04 = 7,
        PANTS05 = 8,
        PANTS05_00_00 = 9,
        PANTS05_00_01 = 10,
        PANTS06 = 11,
        PANTS07 = 12,
        PANTS08 = 13,
        PANTS08_00_00 = 14,
        PANTS08_00_01 = 15,
        PANTS08_00_02 = 16,
        PANTS08_00_03 = 17,
        PANTS09 = 18,
        PANTS10 = 19,
        PANTS11 = 20,
        PANTS70 = 21,
        PANTS71 = 22,
        PANTS72 = 23,
        PANTS73 = 24,
        PANTS74 = 25,
        PANTS75 = 26,
        PANTS76 = 27,
    };
}
namespace app::ropeway::gamemastering::TimelineEventManager {
    enum class CameraEventPauseState : int64_t {
        Wait = 0,
        ExecPause = 1,
        ExecPauseAfter = 2,
        SkipWwise = 3,
        SkipEvent = 4,
        CalcTimeLag = 5,
    };
}
namespace app::ropeway::gimmick::action::RogueItemSpawnGimmick {
    enum class State : int64_t {
        Invalid = 0xFFFFFFFF,
        Initialize = 0,
        Idle = 1,
        Damaged = 2,
        StartFalling = 3,
        Falling = 4,
        EndFalling = 5,
        Finalize = 6,
        Broken = 7,
    };
}
namespace app::ropeway::TerrainAnalyzer {
    enum class PlaneType : int64_t {
        FLOOR = 0,
        SLOPE = 1,
        WALL = 2,
        CROOK = 3,
        CEILING = 4,
    };
}
namespace via::motion::CppSampleAppendChild {
    enum class Test : int64_t {
        A = 0,
        B = 1,
        C = 2,
    };
}
namespace app::ropeway::fsmv2::action::variables::AssignVariableAction {
    enum class AssignType : int64_t {
        Add = 0,
        Sub = 1,
        Mul = 2,
        Div = 3,
    };
}
namespace app::ropeway::survivor::npc::NpcParam {
    enum class DistanceType : int64_t {
        MOVE_START = 0,
        MOVE_END = 1,
        JOG_START = 2,
        WALK_START = 3,
        BACK_MOVE_START = 4,
        BACK_MOVE_END = 5,
        RAY_AVOIDANCE_START = 6,
        NEXT_MOVE_LINE_TARGET = 7,
        RELATIVE_JOS_START = 8,
        RELATIVE_JOS_END = 9,
        JA_STAND_TO_WALK_START = 10,
        JA_WALK_TO_JOG_START = 11,
        JA_JOG_TO_WALK_START = 12,
        JA_WALK_TO_STAND_START = 13,
        ANY = 14,
    };
}
namespace app::ropeway::gui::FloatIconBehavior {
    enum class State : int64_t {
        NONE = 0,
        TO_ON = 1,
        DRAWING = 2,
        TO_OFF = 3,
    };
}
namespace app::ropeway::enemy::tracks::Em7000DecalTrack {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Press = 0,
        Sensor_Touch = 1,
        Attack_BODY_M = 2,
        Attack_BURST = 3,
        Attack_PROUD = 4,
        Attack_Grapple = 5,
        Decal = 6,
    };
}
namespace via::dialog {
    enum class Error : int64_t {
        Nothing = 0,
        NotSupported = 0xFFFFFFFF,
        InvalidParam = -2,
        InvalidState = -3,
        NotRunning = -4,
        UnexpectedFatal = -255,
    };
}
namespace app::ropeway::NpcDefine {
    enum class MoveStateKind : int64_t {
        None = 0,
        Stand = 1,
        Walk = 2,
        Jog = 3,
    };
}
namespace via::wwise {
    enum class EventStatusType : int64_t {
        None = 0,
        Reserved = 1,
        Posted = 2,
        Finished = 3,
        Failed = 4,
        Invalid = 5,
        Max = 6,
    };
}
namespace app::ropeway::UpdateOrderGroup {
    enum class CameraControlPhase : int64_t {
        FASTEST = -41,
        Fixed = -40,
        Player = -39,
        Light = -38,
        RailUserCurve = -37,
        FilterParam = -36,
        FilterLayer = -35,
        FilterApply = -34,
        MaIN_ = -33,
        Mixer = -32,
        LATEST = -31,
    };
}
namespace app::ropeway::enemy::em0000::Em0000FeatherBody {
    enum class DistanceLevel : int64_t {
        LEVEL_0 = 0,
        LEVEL_1 = 1,
        LEVEL_2 = 2,
        LEVEL_3 = 3,
        LEVEL_4 = 4,
        LEVEL_5 = 5,
    };
}
namespace via::hid::hmd::MorpheusDevice {
    enum class StereoEye : int64_t {
        Left = 0,
        Right = 1,
        Count = 2,
    };
}
namespace app::ropeway::enemy::em0000::RoleAction {
    enum class RolePattern : int64_t {
        IDLE = 0,
        LOITERING = 1,
        DEAD_IDLE = 2,
        FAKEDEAD = 3,
        KNOCK = 4,
        ONESHOT = 5,
        LOUNGE = 6,
        EAT = 7,
        WINDOW = 8,
        DOOR = 9,
        RAILING_FALL = 10,
        CAPTURE = 11,
        DEAD = 12,
        DEAD_ACTION = 13,
        EATEN = 14,
        EVENTSET = 15,
    };
}
namespace via {
    enum class SoftwarePrefetchLevel : int64_t {
    };
}
namespace app::ropeway::EnemyDefine {
    enum class LOD : int64_t {
        LEVEL_0 = 0,
        LEVEL_1 = 1,
        LEVEL_2 = 2,
        LEVEL_3 = 3,
        LEVEL_4 = 4,
        LEVEL_5 = 5,
    };
}
namespace app::ropeway::effect::script::EPVExpertBiteData::EPVExpertBiteElement {
    enum class EMIT_DIRECTION : int64_t {
        Z = 0,
        X = 1,
        Y = 2,
    };
}
namespace via::render {
    enum class Topology : int64_t {
        Undefined = 0,
        PointList = 1,
        LineList = 2,
        LineStrip = 3,
        TriangleList = 4,
        TriangleStrip = 5,
        LineListAdj = 6,
        LineStripAdj = 7,
        TriangleListAdj = 8,
        TriangleStripAdj = 9,
        PatchList_ControlPoint1 = 10,
        PatchList_ControlPoint2 = 11,
        PatchList_ControlPoint3 = 12,
        PatchList_ControlPoint4 = 13,
        PatchList_ControlPoint5 = 14,
        PatchList_ControlPoint6 = 15,
        PatchList_ControlPoint7 = 16,
        PatchList_ControlPoint8 = 17,
        PatchList_ControlPoint9 = 18,
        PatchList_ControlPoint10 = 19,
        PatchList_ControlPoint11 = 20,
        PatchList_ControlPoint12 = 21,
        PatchList_ControlPoint13 = 22,
        PatchList_ControlPoint14 = 23,
        PatchList_ControlPoint15 = 24,
        PatchList_ControlPoint16 = 25,
        PatchList_ControlPoint17 = 26,
        PatchList_ControlPoint18 = 27,
        PatchList_ControlPoint19 = 28,
        PatchList_ControlPoint20 = 29,
        PatchList_ControlPoint21 = 30,
        PatchList_ControlPoint22 = 31,
        PatchList_ControlPoint23 = 32,
        PatchList_ControlPoint24 = 33,
        PatchList_ControlPoint25 = 34,
        PatchList_ControlPoint26 = 35,
        PatchList_ControlPoint27 = 36,
        PatchList_ControlPoint28 = 37,
        PatchList_ControlPoint29 = 38,
        PatchList_ControlPoint30 = 39,
        PatchList_ControlPoint31 = 40,
        PatchList_ControlPoint32 = 41,
    };
}
namespace via::nnfc::nfp {
    enum class NfpErrorSelector : int64_t {
        None = 0,
        NotSupport = 1,
        InvalidFormat = 2,
        WirelessOff = 3,
        TagLost = 4,
        TagLostOnFlush = 5,
        NoAppData = 6,
        MismatchApp = 7,
        NeedRegister = 8,
        NeedRestore = 9,
        NeedFormat = 10,
    };
}
namespace app::ropeway::gimmick::action::GimmickPrisonOpenLever {
    enum class ActionResult : int64_t {
        NOT_YET = 0,
        CANCEL = 1,
        ACTION = 2,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class FirstDamagePresetType : int64_t {
        NONE = 0,
        SLASHED_LV1 = 1,
        BULLET_LV1 = 2,
        BULLET_LV2 = 3,
        BULLET_LV3 = 4,
        RAMDOM = 5,
    };
}
namespace via::gui {
    enum class ResolutionAdjustCondition : int64_t {
        Always = 0,
        Expanding = 1,
        Shrinking = 2,
        Max = 3,
    };
}
namespace app::ropeway::OptionManager {
    enum class MeshQuality : int64_t {
        LOW = 0,
        STANDARD = 1,
        HIGH = 2,
        HIGHEST = 3,
    };
}
namespace app::ropeway::environment::GameObjectCullingController {
    enum class Command : int64_t {
        On = 0,
        Off = 1,
    };
}
namespace via::physics::ShapeClosestResult {
    enum class Result : int64_t {
        Success = 0,
        Failure = 1,
    };
}
namespace via::motion::ChainResource {
    enum class ChainAttrFlags : int64_t {
        None = 0,
        ModelCollisionPreset = 1,
    };
}
namespace via::charset::detail {
    enum class ElementType : int64_t {
        Unknown = 0,
        Ascii = 1,
        Kana = 2,
        HanKana = 2,
        Kanji = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickLever {
    enum class StateType : int64_t {
        NONE = 0,
        HOLD = 1,
        TURN_ON = 2,
        TURN_OFF = 3,
        TURNED_ON = 4,
        TURNED_OFF = 5,
        TURNED_ON_AS_SLAVE = 6,
        TURNED_OFF_AS_SLAVE = 7,
    };
}
namespace app::ropeway::PlayerStepTrack {
    enum class StepType : int64_t {
        LEFT = 0,
        RIGHT = 1,
        STAND = 2,
    };
}
namespace app::ropeway::gamemastering::item::DetailCollisionBase {
    enum class MotionString : int64_t {
        Start = 0,
        Start2 = 1,
        Start3 = 2,
        End = 3,
    };
}
namespace app::ropeway::survivor::fsmv2::condition::ActionOrderCondition {
    enum class CheckMoveDirectionType : int64_t {
        None = 0,
        Inside = 1,
        Outside = 2,
    };
}
namespace via::effect {
    enum class PlayerAction : int64_t {
        Idle = 0,
        Start = 1,
        Kill = 2,
        Finish = 3,
        Pause = 4,
        Resume = 5,
        Step = 6,
    };
}
namespace app::ropeway::gui::GimmickNumberLockHothouseGuiBehavior2 {
    enum class JobType : int64_t {
        NONE = 0,
        LADDER = 1,
        POWER = 2,
    };
}
namespace via::network::protocol {
    enum class SendOption : int64_t {
        None = 0,
        Unreliable = 0,
        Unreliable_Low = 0,
        Unreliable_High = 1,
        Reliable = 4,
        ReliableBuffer = 12,
        FastCallback = 16,
        CheckAbsent = 32,
        Tag = 64,
    };
}
namespace app::ropeway::InputDefine {
    enum class AssignType : int64_t {
        XboxOne = 0,
        DualShock4 = 1,
        Type_0 = 2,
        Type_1 = 3,
        Type_2 = 4,
        Type_3 = 5,
        Type_4 = 6,
        Type_5 = 7,
        Vita_1 = 8,
        Vita_2 = 9,
        Vita_3 = 10,
        Vita_4 = 11,
        Vita_5 = 12,
        Vita_6 = 13,
        Vita_7 = 14,
        Vita_8 = 15,
    };
}
namespace app::ropeway::DynamicMotionBankDefine {
    enum class OrderKind : int64_t {
        Default = 0,
        Weapon_BareHand = 1000,
        Outer = -1000,
    };
}
namespace app::ropeway::CollisionDefine {
    enum class AttackShieldingType : int64_t {
        Object = 0,
        Owner = 1,
        Collider = 2,
    };
}
namespace via::motion::JointRemapValue::RemapValueItem {
    enum class TRS : int64_t {
        Trans = 0,
        Rot = 1,
        Scale = 2,
    };
}
namespace app::ropeway::weapon::fsmv2::condition::WeaponCheckTypeFsmCondition {
    enum class CheckTarget : int64_t {
        Type = 0,
        Category = 1,
        TypeOrCategory = 2,
    };
}
namespace app::ropeway::gamemastering::RogueStorageManager {
    enum class QueryResultTypeExist : int64_t {
        InValid = 0xFFFFFFFF,
        ExistAsSingleItem = 0,
        ExistAsMultiItems = 1,
        LackOfCount = 2,
        NotExist = 3,
        CountIsZero = 4,
    };
}
namespace via::motion::Chain {
    enum class GravityCoord : int64_t {
        Default = 0,
        World = 1,
        Local = 2,
    };
}
namespace via::render::RenderTargetOperator {
    enum class CompareFunc : int64_t {
        AlwaysPass = 0,
        AlwaysIgnore = 1,
        GreaterEqual = 2,
        Less = 3,
    };
}
namespace app::ropeway::GimmickDialLockManager {
    enum class RecordDialLock : int64_t {
        NONE = 0xFFFFFFFF,
        ID_00_RPD1F_SAFE = 0,
        ID_01_RPD2F_SAFE = 1,
        ID_02_WASTE_SAFE = 2,
        ID_03_RPD2F_DIAL = 3,
        ID_04_RPD3F_DIAL = 4,
        ID_05_WASTE_DIAL = 5,
        ID_06_RPDLEONDESK_DIAL_A = 6,
        ID_07_RPDLEONDESK_DIAL_B = 7,
        MAX = 8,
    };
}
namespace app::ropeway::enemy::em7000::fsmv2::condition::TargetDirection {
    enum class CompareType : int64_t {
        Equal = 0,
        NotEqual = 1,
        Less = 2,
        LessEq = 3,
        Greater = 4,
        GreaterEq = 5,
    };
}
namespace via::render::RenderOutput {
    enum class RenderMode : int64_t {
        Default = 0,
        LightWeight = 1,
    };
}
namespace app::ropeway::gui::MenuStoryBehavior {
    enum class DecidedResult : int64_t {
        CONTINUE = 0,
        LOAD = 1,
        NEW_GAME = 2,
        NEW_GAME_PLUS = 3,
        CLEAR_STATE = 4,
        COSTUME = 5,
        MAX = 6,
        CANCEL = 7,
    };
}
namespace app::ropeway::IlluminationContainer {
    enum class Status : int64_t {
        On = 0,
        Off = 1,
    };
}
namespace via::effect::script::EPVExpertHitEffectRecordData {
    enum class DECALTOPDIRECTION : int64_t {
        Random = 0,
        DirectionY = 1,
    };
}
namespace app::ropeway::gimmick::action::Trigger {
    enum class TriggerBootType : int64_t {
        Area = 0,
        Key = 1,
        Sight = 2,
        UseItem = 3,
        CheckFlag = 4,
        Unset = 0xFFFFFFFF,
    };
}
namespace via::movie::Movie {
    enum class PlayState : int64_t {
        Idle = 0,
        InitializeStart = 1,
        Initialized = 2,
        BufferingStart = 3,
        Buffering = 4,
        Buffered = 5,
        PlayStart = 6,
        Playing = 7,
        PauseStart = 8,
        Paused = 9,
        StopStart = 10,
        Stopped = 11,
        SoftStopStart = 12,
        SoftStopped = 13,
        VissStart = 14,
        Finalize = 15,
    };
}
namespace app::ropeway::enemy::em7300::Em7300Think {
    enum class WallAttackStateEnum : int64_t {
        START = 0,
        GOTO_WALL = 1,
        WALL_KICK = 2,
        GOTO_TARGET = 3,
        GOTO_FRONT_POS = 4,
        END = 5,
    };
}
namespace app::ropeway::SurvivorStandbyCatalog {
    enum class CastType : int64_t {
        Player = 0,
        Npc = 1,
        Actor = 2,
    };
}
namespace app::ropeway::fsmv2::CharacterControllerSwitch {
    enum class WarpMode : int64_t {
        Use = 0,
        NotUse = 1,
        UseFromAT = 2,
    };
}
namespace via::hid {
    enum class NpadJoyDeviceType : int64_t {
        Left = 0,
        Right = 1,
    };
}
namespace app::ropeway::gimmick::action::RestageEventFinished {
    enum class AdaptScenarioPattern : int64_t {
        PATTERN_A = 0,
        PATTERN_B = 1,
        PATTERN_C = 2,
    };
}
namespace app::ropeway::survivor::fsmv2::action::SurvivorOrientAction {
    enum class Source : int64_t {
        Front = 1,
        Back = 2,
        Left = 4,
        Right = 8,
    };
}
namespace app::ropeway::InputDefine {
    enum class KeyAssignType : int64_t {
        Type_0 = 0,
        Type_1 = 1,
        Type_2 = 2,
        Type_3 = 3,
        Type_4 = 4,
        Type_5 = 5,
    };
}
namespace app::ropeway::enemy::em7200::Em7200Gimmick {
    enum class GimmickType : int64_t {
        WALL_BREAK = 0,
        JUMP = 1,
        DAMAGE_BREAK = 2,
    };
}
namespace via::effect::script {
    enum class CameraID : int64_t {
        MainCamera = 0,
    };
}
namespace app::ropeway::weapon::shell::ShellBase{
    enum class BulletShellStatusInformation____ :  int64_t {
        NoSuspend = 0,
        Update = 1,
        LateUpdate = 2,
    };
}
namespace app::ropeway::enemy::em0000::RoleMotionID {
    enum class DeadIdle : int64_t {
        DEAD_IDLE_00 = 0,
        DEAD_IDLE_01 = 1,
        DEAD_IDLE_02 = 2,
        DEAD_IDLE_03 = 3,
        DEAD_IDLE_04 = 4,
        UNIQUE_00 = 5,
    };
}
namespace app::ropeway::ChoreographPlayer::ChoreographParamCurveAnimator {
    enum class ActionType : int64_t {
        START_LOOP = 0,
        LOOP = 1,
    };
}
namespace app::ropeway {
    enum class CommonlySoundAppealType : int64_t {
        SILENT = 0,
        WHISPER = 1,
        LIGHTLY = 2,
        MIDDLE = 3,
        HEAVY = 4,
        ROAR = 5,
    };
}
namespace app::ropeway::enemy::em7000::Em7000Define {
    enum class GModePhase : int64_t {
        GModePhase_William_00 = 0,
        GModePhase_G1_00 = 1,
        GModePhase_William_01 = 2,
        GModePhase_G1_01 = 3,
        GModePhase_BattleStart = 4,
    };
}
namespace via::motion::ContinueOptions {
    enum class ContinueType : int64_t {
        Non = 0,
        PrevEnd = 1,
        ExitFrame = 2,
    };
}
namespace app::ropeway::enemy::em7100 {
    enum class ThinkPhase : int64_t {
        ThinkPhase_Start = 0,
        ThinkPhase_Celling = 1,
        ThinkPhase_Before_Aisle = 2,
        ThinkPhase_Aisle = 3,
        ThinkPhase_Crane = 4,
        ThinkPhase_Default = 5,
    };
}
namespace app::ropeway::IkTwoArm {
    enum class TargetPositionType : int64_t {
        WORLD = 0,
        LOCAL = 1,
    };
}
namespace via::effect::script::EffectTimeline {
    enum class TimeLineBindType : int64_t {
        Children = 0,
        Scene = 1,
        Fixed = 2,
        Direct = 3,
        Parent = 4,
    };
}
namespace app::ropeway::Em6000Think {
    enum class ENABLE_ACTION : int64_t {
        Anythig = 0,
        Punch = 1,
        DashPunch = 2,
        BackSlap = 3,
        BackSlash = 4,
        NearHold = 5,
        FarHold = 6,
        RightHook = 7,
        Upper = 8,
        VomitChildren = 9,
        VomitCancel = 10,
        Threat = 11,
        StepThreat = 12,
        ThreatIdle = 13,
        Hide = 14,
    };
}
namespace app::ropeway::DynamicsManager {
    enum class PauseCause : int64_t {
        General = 1,
        Restrict = 2,
    };
}
namespace System::Collections::Generic {
    enum class TreeRotation : int64_t {
        Left = 0,
        LeftRight = 1,
        Right = 2,
        RightLeft = 3,
    };
}
namespace app::ropeway::gimmick::action::GimmickWindow {
    enum class BarricadeType : int64_t {
        None = 0,
        OldBarricade = 1,
        NewBarricade = 2,
    };
}
namespace app::ropeway::rogue::RogueDefine {
    enum class GameMode : int64_t {
        None = 0xFFFFFFFF,
        Short = 0,
        Middle = 1,
        Long = 2,
    };
}
namespace via::motion::script::CharacterSetting {
    enum class ContactAdjustTypeEnum : int64_t {
        Default = 0,
        Scale = 1,
    };
}
namespace app::ropeway::weapon::shell::ShellBase{
    enum class SparkShellStatusInformation :  int64_t {
        NoSuspend = 0,
        Update = 1,
        LateUpdate = 2,
    };
}
namespace app::ropeway::enemy::em7100 {
    enum class ThinkActionID : int64_t {
        THINK_ACT_NONE = 0,
        THINK_ACT_ZERO = 1,
        THINK_ACT_NORMAL = 2,
        THINK_ACT_COMBO = 3,
        THINK_ACT_BACK_R = 4,
        THINK_ACT_BACK_L = 5,
        THINK_ACT_NORMAL_RL = 6,
        THINK_ACT_NORMAL_RH = 7,
        THINK_ACT_VERTICAL = 8,
        THINK_ACT_LONG_MURDER = 9,
        THINK_ACT_LONG_MURDER_R90 = 10,
        THINK_ACT_LONG_MURDER_COMBO = 11,
        THINK_ACT_NORMAL_RHQ = 12,
        THINK_ACT_NEAR_CRAW_TYPE2 = 13,
        THINK_ACT_HEAVY_RL = 14,
        THINK_ACT_HEAVY_RH = 15,
        THINK_ACT_HEAVY_VERTICAL = 16,
        THINK_ACT_DEMO_00 = 17,
        THINK_ACT_DOWN_ATTACK = 18,
        THINK_ACT_GRAPPLE = 19,
        THINK_ACT_DOWN_ATTACK_V2 = 20,
        THINK_ACT_CONTINUE_REACTION = 21,
    };
}
namespace via::navigation::map {
    enum class SectionType : int64_t {
        NoSection = 0,
        Owner = 1,
        Section = 2,
        ConnectManager = 3,
        IndividualSection = 4,
        Invalid = 0xFFFFFFFF,
    };
}
namespace via::render::detail {
    enum class PrimDrawType : int64_t {
        Unknown = 0,
        Billboard2D = 1,
        Billboard3D = 2,
        Polygon = 3,
        PolygonStrip = 4,
        Ribbon = 5,
        Mesh = 6,
        MeshInstancing = 7,
        Fluid2D = 8,
        GUI = 9,
        GUIMaterial = 10,
        LensflareIris = 11,
        LensflareHoop = 12,
        LensflareGeneric = 13,
        LensflareDebug = 14,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class ConditionStateBitFlag : int64_t {
        IN_SIGHT = 0,
        SAME_AREA = 1,
        ENABLE_THINK = 2,
        ENABLE_ATTACK = 3,
        ENABLE_ADDREACTION = 4,
        ENABLE_SOUND = 5,
        HIDE_BGM = 6,
        HIDE_SE = 7,
        TRANSFIX_PLAY_SE = 8,
        PLAYING_ROLE = 9,
        PLAYING_RELOCATION = 10,
        PLAYING_ACTOR = 11,
        FORBID_RELOCATION = 12,
        FORBID_RELOCATION_LOITERING = 13,
        FORBID_SAVE_POSITION = 14,
        FORBID_SAVE_POSITION_ZONE = 15,
        BEWAREDED = 16,
        KEEP_ATTENTION = 17,
        KEEP_HATE = 18,
        KEEP_FIND_STATE = 19,
        KEEP_FINDING_STATE = 20,
        SENSOR_OFF = 21,
        NO_ADD_ATTENTION = 22,
        JACKED_MOTION = 23,
        CONTROLED_FSM = 24,
        CONTROLED_GIMMICK = 25,
        CONTROLED_EVENT = 26,
        FIRST_AVAILABLE_POS_SAVE = 27,
    };
}
namespace app::ropeway::ParamCurveAnimator::ProcessLight {
    enum class ParamType : int64_t {
        INTENSITY = 0,
        EFFECTIVE_RANGE = 1,
    };
}
namespace app::ropeway::enemy::em4100::behaviortree::condition::Em4100BtCondition_CheckDemoFlyMode {
    enum class ExpressionType : int64_t {
        EQ = 0,
        NE = 1,
    };
}
namespace via::detail_qt {
    enum class qt_cell_link_bit : int64_t {
        qt_cell_link_bit_none = 0,
        qt_cell_link_bit_00 = 1,
        qt_cell_link_bit_01 = 2,
        qt_cell_link_bit_10 = 4,
        qt_cell_link_bit_11 = 8,
    };
}
namespace via {
    enum class SharePlayConnectionStatus : int64_t {
        Disable = 0xFFFFFFFF,
        Failed = -2,
        Dormant = 0,
        Ready = 1,
        Connected = 2,
    };
}
namespace app::ropeway::survivor::tag {
    enum class ActConditionAttribute : int64_t {
        HOLD = 3300504884,
        REACTION_DOWN = 1943340773,
        REACTION_STAND = 1214026829,
        DEAD = 2846171846,
        QUICK_TURN = 1677611524,
        COVER_START = 2588540973,
        COVER_HOLD = 1337625907,
        COVER_END = 394693696,
    };
}
namespace via::navigation::RelationMapInfo {
    enum class Attribute : int64_t {
        UseOpimizedData = 0,
        AttributeNum = 1,
    };
}
namespace app::ropeway::SceneStandbyController {
    enum class Phase : int64_t {
        None = 0,
        NoneToStandaby = 1,
        StandbyToNone = 2,
        Standby = 3,
        StandbyToActive = 4,
        ActiveToStandby = 5,
        Active = 6,
    };
}
namespace app::ropeway::gimmick::action::GimmickBreakableColumnForG2Battle {
    enum class State : int64_t {
        Wait = 0,
        Break = 1,
    };
}
namespace app::ropeway::OptionManager {
    enum class DefaultResolutionMaxType : int64_t {
        INFINITE_ = 0,
        MAX1980P = 1,
        MAX3840P = 2,
    };
}
namespace via::gui {
    enum class LightGroupId : int64_t {
        Group0 = 0,
        Group1 = 1,
        Group2 = 2,
        Group3 = 3,
        Group4 = 4,
        Group5 = 5,
        Group6 = 6,
        Group7 = 7,
    };
}
namespace app::ropeway::gimmick::action::GimmickWiringNodeBase {
    enum class RoutineType : int64_t {
        INIT = 0,
        WAIT = 1,
    };
}
namespace app::ropeway::InputDefine::AnalogStick {
    enum class Power : int64_t {
        NONE = 0,
        LOW = 1,
        HIGH = 2,
    };
}
namespace via::gui {
    enum class ControlPoint : int64_t {
        LeftTop = 0,
        LeftCenter = 4,
        LeftBottom = 8,
        CenterTop = 1,
        CenterCenter = 5,
        CenterBottom = 9,
        RightTop = 2,
        RightCenter = 6,
        RightBottom = 10,
    };
}
namespace app::ropeway::interpolate {
    enum class InterpolatePriority : int64_t {
        HIGHEST = 0,
        SCALE = 1,
        ROTATION = 2,
        TILT = 3,
        POSITION = 4,
        CONST_ = 5,
        LOWEST = 6,
    };
}
namespace app::ropeway::enemy::tracks::Em4000ColliderTrack {
    enum class ColliderGroup : int64_t {
        Invalid = 0xFFFFFFFF,
        Press = 0,
        Damage = 1,
        Sensor_Touch = 2,
        Attack = 3,
        Hold = 4,
        CityAttack = 5,
        Sensor_Gimmick = 6,
        DrawControl = 7,
        PressDoor = 8,
        AimTarget = 9,
    };
}
namespace app::ropeway::camera::TitleCameraController {
    enum class TitleScene : int64_t {
        TITLE_SCENE_MAIN_ = 0,
        TITLE_SCENE_GAS_STATION = 1,
        TITLE_SCENE_OPENING = 2,
        TITLE_SCENE_RPD = 3,
        TITLE_SCENE_RPD_UNDERGROUND = 4,
        TITLE_SCENE_WASTE_WATER = 5,
        TITLE_SCENE_WATER_PLANT = 6,
        TITLE_SCENE_ORPHAN_ASYLUM = 7,
        TITLE_SCENE_ORPHAN_APPROACH = 8,
        TITLE_SCENE_LABORATORY = 9,
        TITLE_SCENE_TRANSPORTATION = 10,
        TITLE_SCENE_LATEST = 11,
    };
}
namespace via::effect::detail {
    enum class EmitterContextType : int64_t {
        Unknown = 0,
        Billboard2D = 1,
        Billboard3D = 2,
        RibbonFollow = 3,
        RibbonLength = 4,
        RibbonChaIN_ = 5,
        Mesh = 6,
        NodeBillboard = 7,
        StrainRibbon = 8,
        NoDraw = 9,
        Polygon = 10,
        RibbonTrail = 11,
        PolygonTrail = 12,
        GpuBillboard = 13,
        RibbonFixEnd = 14,
        GpuRibbonFollow = 15,
        RibbonLightweight = 16,
        Lightning3D = 17,
    };
}
namespace via::motion::detail::ChainNodeData {
    enum class AttrFlags : int64_t {
        None = 0,
        PartMotionBlend = 1,
    };
}
namespace via::dynamics {
    enum class RayCastOption : int64_t {
        AllHits = 0,
        DisableBackFacingTriangleHits = 1,
        DisableFrontFacingTriangleHits = 2,
        BackFacingTriangleHits = 3,
        FrontFacingTriangleHits = 4,
        NearSort = 5,
        InsideHits = 6,
        OneHitBreak = 7,
        Max = 8,
    };
}
namespace via {
    enum class UserIndex : int64_t {
        User0 = 0,
        User1 = 1,
        User2 = 2,
        User3 = 3,
        User4 = 4,
        User5 = 5,
        User6 = 6,
        User7 = 7,
        User8 = 8,
        User9 = 9,
        User10 = 10,
        User11 = 11,
        User12 = 12,
        User13 = 13,
        User14 = 14,
        User15 = 15,
        Reserved = 16,
        Max = 17,
        System = 18,
        Invalid = 19,
        Merged = 20,
    };
}
namespace app::ropeway::gamemastering::TutorialManager {
    enum class EndCondition : int64_t {
        TIME = 0,
        FLAG = 1,
        FLAG_OR_TIME = 2,
    };
}
namespace app::ropeway::fsmv2::enemy::action::Em7200FsmAction_TimerCount {
    enum class TimerTypeEnum : int64_t {
        RankLoopTimer = 0,
    };
}
namespace via::relib::effect::EffectControlAction {
    enum class TargetGameObjectModeEnum : int64_t {
        GameObjectRef = 0,
        OwnGameObject = 1,
    };
}
namespace app::ropeway::enemy::em0000 {
    enum class StandType : int64_t {
        STANDING = 0,
        KNEELING = 1,
        CREEPING = 2,
    };
}
namespace via::render {
    enum class CullMode : int64_t {
        None = 1,
        Front = 2,
        Back = 3,
        Num = 4,
    };
}
namespace app::ropeway::gimmick::option::TextMessageSettings::DispMessagesWork {
    enum class Routine : int64_t {
        CHECK = 0,
        DRAW = 1,
        WAIT = 2,
        ERROR_ = 3,
    };
}
namespace app::ropeway::EquipmentDefine {
    enum class WeaponCategory : int64_t {
        Invalid = 0xFFFFFFFF,
        Handgun = 0,
        Shotgun = 1,
        Submachinegun = 2,
        Magnum = 3,
        GrenadeLauncher = 4,
        SparkShot = 5,
        Flamethrower = 6,
        RocketLauncher = 7,
        Gatlinggun = 8,
        Knife = 9,
        Grenade = 10,
        FlashGrenade = 11,
        Stungun = 12,
        Bottle = 13,
    };
}
namespace app::ropeway::gui::FloorMapSt5ABehavior {
    enum class DoorId : int64_t {
        Door_invalid = 0,
        sm40_058_PlantOfficeDoor_N01 = 1,
        sm40_058_PlantOfficeDoor_N02 = 2,
        sm40_137_AutoDoor1m02A_N01 = 3,
        sm40_137_AutoDoor1m02A_N02 = 4,
        sm40_137_AutoDoor1m02A_N03 = 5,
        sm40_137_AutoDoor1m02A_N04 = 6,
        sm40_146_AutoDoorCommon01A_N05 = 7,
        sm40_146_AutoDoorCommon01A_G01 = 8,
        sm40_146_AutoDoorCommon01A_G03 = 9,
        sm40_146_AutoDoorCommon01A_G04 = 10,
        sm40_146_AutoDoorCommon01A_G05 = 11,
        sm40_146_AutoDoorCommon01A_G06 = 12,
        sm40_146_AutoDoorCommon01A_T01 = 13,
        sm40_137_AutoDoor1m02A_T02 = 14,
        sm40_058_PlantOfficeDoor_T01 = 15,
        sm40_137_AutoDoor1m02A_T03 = 16,
        sm40_143_IvyDoor01A_T05 = 17,
        sm40_062_IronDoor_T04 = 18,
        sm40_062_IronDoor_T05 = 19,
        sm40_062_IronDoor_T07 = 20,
    };
}
namespace via::physics {
    enum class RequestState : int64_t {
        None = 0,
        Faulted = 1,
        RanToCompletion = 2,
        Running = 3,
        WaitingToRun = 4,
    };
}
namespace app::ropeway::survivor::npc::moveline::Point {
    enum class PointTypeKind : int64_t {
        Anchor = 0,
        Control = 1,
        Invalid = 2,
    };
}
namespace app::ropeway::BodyPartsBreakInfo {
    enum class PartsType : int64_t {
        CATEGORY = 0,
        DETAIL = 1,
    };
}
namespace app::ropeway::enemy::em4000::Em4000Gimmick {
    enum class AreaTypeEnum : int64_t {
        NO_CHECK = 0,
        EM_AREA_IN_ = 1,
        PL_EM_CIRCLE_IN_ = 2,
    };
}
namespace via::storage::saveService {
    enum class SaveSlot : int64_t {
        Auto = 0,
        SystemMaxOffset = -128,
        System = 0xFFFFFFFF,
        Slot = 1,
        SlotMax = 256,
    };
}
namespace via::effect::script {
    enum class Group : int64_t {
        Player = 0,
        Enemy = 1,
        Weapon = 2,
        Item = 3,
        Prop = 4,
        Etc = 5,
        Vfx = 6,
        Camera = 7,
        ActionPoint = 8,
        EventChara = 9,
    };
}
namespace app::ropeway::enemy::em3000 {
    enum class MoveType : int64_t {
        AUTO = 0,
        WALK = 1,
        RUN = 2,
        CAUTION = 3,
        CAUTION_RANDOM = 4,
        UNIQUE = 5,
    };
}
namespace app::ropeway::FacialController {
    enum class Layer : int64_t {
        EVENT = 0,
        BREATHING = 1,
        EMOTION_STATUS = 2,
        SITUATION = 3,
        NUM = 4,
    };
}
namespace app::ropeway::gimmick::action::GimmickCableCar {
    enum class RoutineMineType : int64_t {
        WAIT = 0,
        PLAY = 1,
    };
}
namespace app::ropeway::gui::SelectHDRBrightnessBehaviorMax {
    enum class EndType : int64_t {
        SELECTED = 0,
        CANCELED = 1,
    };
}
namespace via::render {
    enum class MaterialShadingType : int64_t {
        Standard = 0,
        Decal = 1,
        DecalWithMetallic = 2,
        DecalNRMR = 3,
        Transparent = 4,
        Distortion = 5,
        PrimitiveMesh = 6,
        Water = 7,
        GUI = 8,
        GUIMesh = 9,
        ExpensiveTransparent = 10,
        Forward = 11,
    };
}
namespace via::motion::tree::GamePadTriggerNode {
    enum class TriggerKind : int64_t {
        Left = 0,
        Right = 1,
    };
}
namespace app::ropeway::enemy::em6300::MotionPattern {
    enum class DirLR : int64_t {
        Left = 0,
        Right = 1,
    };
}
namespace app::ropeway::gui::GimmickSafeBoxDialBehavior {
    enum class State : int64_t {
        NoMove = 0,
        Wait = 1,
        Left = 2,
        Right = 3,
        Reset = 4,
    };
}
namespace via::render {
    enum class HDROutputFormat : int64_t {
        None = 0,
        HDR10 = 1,
        scRGB = 2,
    };
}
namespace via::network::session {
    enum class CompOperator : int64_t {
        None = 0,
        EQ = 1,
        NE = 2,
        GT = 3,
        GE = 4,
        LT = 5,
        LE = 6,
    };
}
namespace app::ropeway::EnemyDefine {
    enum class ForceMoveFactor : int64_t {
        Unnown = 0,
        Fsm = 1,
        Loitering = 2,
        Around = 3,
        AreaRestrict = 4,
        DoorLeave = 5,
        Chaser = 6,
        OutOfTerritory = 7,
    };
}
namespace System::Reflection {
    enum class ParameterAttributes : int64_t {
        None = 0,
        IN_ = 1,
        Out = 2,
        Lcid = 4,
        Retval = 8,
        Optional = 16,
        HasDefault = 4096,
        HasFieldMarshal = 8192,
        Reserved3 = 16384,
        Reserved4 = 32768,
        ReservedMask = 61440,
    };
}
namespace app::ropeway::gui::FloorMapSt43Behavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_4_103_0 = 1,
        Room_4_601_0a = 2,
        Room_4_601_0b = 3,
        Room_4_602_0 = 4,
        Room_4_603_0a = 5,
        Room_4_603_0b = 6,
        Room_4_603_0c = 7,
        Room_4_604_0 = 8,
        Room_4_607_0 = 9,
        Room_4_608_0 = 10,
        Room_4_608_0b = 11,
        Room_4_609_0 = 12,
        Room_4_610_0 = 13,
    };
}
namespace app::ropeway::gamemastering::SaveDataManager {
    enum class AnalyzedSaveFileTextList : int64_t {
        SCENARIO_TYPE = 0,
        DIFFICULTY = 1,
        SAVE_TIMES = 2,
        LOCATION_ID = 3,
        AREA_ID = 4,
        PURPOSE = 5,
        PLAYER_NAME = 6,
        COUNT = 7,
    };
}
namespace via::render::Shadow {
    enum class ShadowPriority : int64_t {
        Low = 0,
        Normal = 1,
        High = 2,
        Max = 3,
    };
}
namespace via::effect::graph {
    enum class ItemType : int64_t {
        Unknown = 0,
        Spawn = 1,
        SpawnExpression = 2,
        Transform2D = 3,
        Transform2DModifier = 4,
        Transform2DClip = 5,
        Transform2DExpression = 6,
        Transform3D = 7,
        Transform3DModifier = 8,
        Transform3DClip = 9,
        Transform3DExpression = 10,
        ParentOptions = 11,
        FixRandomGenerator = 12,
        TypeBillboard2D = 13,
        TypeBillboard2DExpression = 14,
        TypeBillboard3D = 15,
        TypeBillboard3DExpression = 16,
        TypeMesh = 17,
        TypeMeshClip = 18,
        TypeMeshExpression = 19,
        TypeRibbonFollow = 20,
        TypeRibbonLength = 21,
        TypeRibbonChaIN_ = 22,
        TypeRibbonFixEnd = 23,
        TypeRibbonLightweight = 24,
        TypeRibbonFollowExpression = 25,
        TypeRibbonLengthExpression = 26,
        TypeRibbonChainExpression = 27,
        TypeRibbonFixEndExpression = 28,
        TypePolygon = 29,
        TypePolygonClip = 30,
        TypePolygonExpression = 31,
        TypeRibbonTrail = 32,
        TypePolygonTrail = 33,
        TypeNoDraw = 34,
        TypeNoDrawExpression = 35,
        Velocity2D = 36,
        Velocity2DExpression = 37,
        Velocity3D = 38,
        Velocity3DExpression = 39,
        RotateAnim = 40,
        RotateAnimExpression = 41,
        ScaleAnim = 42,
        ScaleAnimExpression = 43,
        Life = 44,
        LifeExpression = 45,
        UVSequence = 46,
        UVSequenceExpression = 47,
        UVScroll = 48,
        TextureUnit = 49,
        EmitterShape2D = 50,
        EmitterShape2DExpression = 51,
        EmitterShape3D = 52,
        EmitterShape3DExpression = 53,
        AlphaCorrection = 54,
        TypeStrainRibbon = 55,
        TypeStrainRibbonExpression = 56,
        TypeLightning3D = 57,
        ShaderSettings = 58,
        ShaderSettingsExpression = 59,
        Distortion = 60,
        RenderTarget = 61,
        PtLife = 62,
        PtBehavior = 63,
        PtBehaviorClip = 64,
        PlayEfx = 65,
        FadeByAngle = 66,
        FadeByAngleExpression = 67,
        FadeByEmitterAngle = 68,
        FadeByDepth = 69,
        FadeByDepthExpression = 70,
        FadeByOcclusion = 71,
        FadeByOcclusionExpression = 72,
        FakeDoF = 73,
        LuminanceBleed = 74,
        TypeNodeBillboard = 75,
        TypeNodeBillboardExpression = 76,
        UnitCulling = 77,
        FluidEmitter2D = 78,
        FluidSimulator2D = 79,
        PlayEmitter = 80,
        PtTransform3D = 81,
        PtTransform3DClip = 82,
        PtTransform2D = 83,
        PtTransform2DClip = 84,
        PtVelocity3D = 85,
        PtVelocity3DClip = 86,
        PtVelocity2D = 87,
        PtVelocity2DClip = 88,
        PtColliderAction = 89,
        PtCollision = 90,
        PtColor = 91,
        PtColorClip = 92,
        PtUvSequence = 93,
        PtUvSequenceClip = 94,
        MeshEmitter = 95,
        MeshEmitterClip = 96,
        MeshEmitterExpression = 97,
        VectorFieldParameter = 98,
        VectorFieldParameterClip = 99,
        VectorFieldParameterExpression = 100,
        DepthOperator = 101,
        ShapeOperator = 102,
        ShapeOperatorExpression = 103,
        WindInfluence3D = 104,
        TypeGpuBillboard = 105,
        TypeGpuBillboardExpression = 106,
        TypeGpuRibbonFollow = 107,
        EmitterPriority = 108,
        DrawOverlay = 109,
        VectorField = 110,
        VolumeField = 111,
        AngularVelocity3D = 112,
        AngularVelocity2D = 113,
        IgnorePlayerColor = 114,
        ProceduralDistortion = 115,
        ProceduralDistortionClip = 116,
        ItemNum = 117,
    };
}
namespace System::Runtime::CompilerServices {
    enum class MethodImplOptions : int64_t {
        Unmanaged = 4,
        NoInlining = 8,
        ForwardRef = 16,
        Synchronized = 32,
        NoOptimization = 64,
        PreserveSig = 128,
        AggressiveInlining = 256,
        InternalCall = 4096,
    };
}
namespace via::nnfc::nfp {
    enum class ErrorDialogCode : int64_t {
        WirelessOff = 0,
        NotSupport = 1,
        InvalidFormatVer = 2,
    };
}
namespace via::physics::ShapeCastResult {
    enum class Result : int64_t {
        Success = 0,
        Failure = 1,
    };
}
namespace via::Window::MessageArgs {
    enum class Button : int64_t {
        L = 0,
        R = 1,
        M = 2,
        EX0 = 3,
        EX1 = 4,
    };
}
namespace app::ropeway::gui::FloorMapSt1CBehavior {
    enum class RoomId : int64_t {
        Room_invalid = 0,
        Room_1_620_0 = 1,
        Room_1_621_0a = 2,
        Room_1_621_0b = 3,
        Room_1_622_0 = 4,
        Room_1_623_0 = 5,
    };
}
namespace app::ropeway::camera {
    enum class CameraTargetType : int64_t {
        INVALID = 0xFFFFFFFF,
        OBJECT = 0,
        POSITION = 1,
    };
}
namespace via::ThreadPool::Worker {
    enum class State : int64_t {
        Uninitialized = 0,
        Idle = 1,
        Assigned = 2,
        Executing = 3,
        Halt = 4,
        Terminated = 5,
    };
}
namespace via::effect::detail {
    enum class MieLightingType : int64_t {
        None = 0,
        Vertex = 1,
        Vertex2x2 = 2,
        Vertex4x4 = 3,
        Vertex8x8 = 4,
        Vertex16x16 = 5,
        Vertex32x32 = 6,
        ForceWord = 0xFFFFFFFF,
    };
}
namespace app::ropeway::fsmv2::PlayMaterialParamAnimator {
    enum class TargetType : int64_t {
        TARGET_REF = 0,
        SELF = 1,
        PARENT = 2,
    };
}
namespace via::effect::script::EffectDecal {
    enum class DecalUpEnum : int64_t {
        XPlus = 0,
        XMinus = 1,
        YPlus = 2,
        YMinus = 3,
        ZPlus = 4,
        ZMinus = 5,
    };
}
namespace app::ropeway::gimmick::action::GimmickBlastBoilerControl {
    enum class BoilerSetType : int64_t {
        SET_TYPE_LEON = 0,
        SET_TYPE_CLAIRE = 1,
    };
}
namespace via::autoplay::action::AutoAttack {
    enum class TurningSpeed : int64_t {
        Turning_Fast = 0,
        Turning_Normal = 1,
        Turning_Slow = 2,
    };
}
namespace via::fsm::TreeNode {
    enum class Attribute : int64_t {
        BranchOnly = 0,
        DisableNetworkSync = 1,
        AttributeBitNum = 32,
    };
}
namespace app::ropeway::enemy::em0000::fsmv2::action::Em0000MFsmAction_DrawFloatIcon {
    enum class FloatIconType : int64_t {
        SCUFFLE_COMMAND = 0,
    };
}
namespace via::hid {
    enum class MouseButton : int64_t {
        NONE = 0,
        L = 1,
        R = 2,
        C = 4,
        UP = 8,
        DOWN = 16,
        EX0 = 32,
        EX1 = 64,
    };
}
namespace app::ropeway::SurvivorDefine {
    enum class Costume : int64_t {
        Invalid = 0xFFFFFFFF,
        Default = 0,
        Costume_1 = 1,
        Costume_2 = 2,
        Costume_3 = 3,
        Costume_4 = 4,
        Costume_5 = 5,
        Costume_6 = 6,
        Costume_7 = 7,
        Costume_8 = 8,
        Costume_9 = 9,
        Costume_A = 10,
        Costume_B = 11,
        Costume_C = 12,
        Costume_D = 13,
        Costume_E = 14,
        Costume_F = 15,
        Scenario = 16,
        Classic = 17,
    };
}
namespace app::ropeway::timeline::TimelineEventSettingTable::FlowSetting::DispEquipmentParam {
    enum class Mode : int64_t {
        System = 0,
        Manual = 1,
    };
}
namespace app::ropeway::OptionManager {
    enum class ColorSpaceType : int64_t {
        sRGB = 0,
        Rec709 = 1,
    };
}
namespace app::ropeway::gimmick::action::GimmickBreakableColumnForG2Battle {
    enum class RigidBodyState : int64_t {
        Inititlize = 0,
        Break = 1,
        BreakStop = 2,
    };
}
namespace app::ropeway::camera::CameraDefine {
    enum class Request : int64_t {
        CUT_SCENE = 0,
        GIMMICK = 1,
        ACTION = 2,
    };
}
namespace app::ropeway::enemy::em7100::BehaviorTreeAction {
    enum class ID : int64_t {
        ACT_IDLE = 0,
        ACT_WALK = 1,
        ACT_RUN = 2,
        ACT_WAIT_ATTACK = 3,
        ACT_CONTINUE_REACTION = 4,
        ACT_DEMO_00 = 5,
        ATK_HIGH_RQ = 2000,
        ATK_HIGH_R = 2001,
        ATK_LOW_L = 2002,
        ATK_LOW_R = 2003,
        ATK_COMOB_2 = 2004,
        ATK_COMBO_3 = 2005,
        ATK_TURN_L = 2006,
        ATK_TURN_R = 2007,
        ATK_ZERO = 2008,
        ATK_VIRTICAL = 2009,
        ATK_COMOB_5 = 2010,
        ATK_COMBO_8 = 2011,
        ATK_JUMP = 2012,
        ATK_LONG_MURDER = 2013,
        ATK_LONG_MURDER_R90 = 2014,
        ATK_LONG_MURDER_COMBO = 2015,
        ATK_VIRTICAL_TYPE2 = 2017,
        ATK_LOW_R_HEAVY = 2018,
        ATK_HIGH_R_HEAVY = 2019,
        ATK_VIRTICAL_HEAVY = 2020,
        ATK_DOWN_ATTACK = 2021,
        ATK_GRAPPLE = 2022,
        ATK_DOWN_ATTACK_V2 = 2023,
    };
}
namespace app::ropeway::gimmick::action::GimmickWiringNodeBase {
    enum class NodeType : int64_t {
        LINE = 0,
        POWER = 1,
        SWITCH = 2,
        MOTOR = 3,
    };
}
namespace app::ropeway::survivor::fsmv2::condition::ActionOrderCondition {
    enum class WeaponCheckTarget : int64_t {
        Type = 0,
        Category = 1,
        TypeOrCategory = 2,
        EquipCategory = 3,
    };
}
namespace app::ropeway::survivor::player::PlayerFacialController {
    enum class BreathingMode : int64_t {
        NONE = 0xFFFFFFFF,
        RUN = 0,
        NUM = 1,
    };
}
namespace via::motion {
    enum class JointGroup : int64_t {
        Default = 0,
        Group1 = 1,
        Group2 = 2,
        Group3 = 3,
        Group4 = 4,
        Group5 = 5,
        Group6 = 6,
    };
}
namespace via::os::http_client {
    enum class RedirectPolicy : int64_t {
        Never = 0,
        Always = 1,
        DisallowHttpsToHttp = 2,
        NoSchemeChanges = 3,
        MAX = 4,
    };
}
namespace via::clr {
    enum class PropertyFlag : int64_t {
        SpecialName = 512,
        RTSpecialName = 1024,
        HasDefault = 4096,
        ExposeMember = 16384,
    };
}
namespace app::ropeway::posteffect::ChangePlayerGameCameraParam {
    enum class SettingMethod : int64_t {
        Param = 0,
        Default = 1,
    };
}
namespace via::network::context {
    enum class Option : int64_t {
        None = 0,
        WithTicket = 1,
        OnlyTicket = 2,
    };
}
namespace via::geometry {
    enum class BrushType : int64_t {
        Additive = 0,
        Subtractive = 1,
    };
}
namespace app::ropeway::CollisionDefine {
    enum class HitPriority : int64_t {
        Priority_00 = 0,
        Priority_01 = 1,
        Priority_02 = 2,
        Priority_03 = 3,
        Priority_04 = 4,
        Priority_05 = 5,
        Priority_06 = 6,
        Priority_07 = 7,
        Priority_08 = 8,
        Priority_09 = 9,
        Priority_10 = 10,
    };
}
namespace via::timeline::UserCurvePlayer {
    enum class DevPlayState : int64_t {
        Disable = 0,
        Play = 1,
        Pause = 2,
    };
}
namespace app::ropeway::OptionManager {
    enum class ImageQuality_Value : int64_t {
        ImageQuality_05 = 0,
        ImageQuality_06 = 1,
        ImageQuality_07 = 2,
        ImageQuality_08 = 3,
        ImageQuality_09 = 4,
        ImageQuality_10 = 5,
        ImageQuality_11 = 6,
        ImageQuality_12 = 7,
        ImageQuality_13 = 8,
        ImageQuality_14 = 9,
        ImageQuality_15 = 10,
        ImageQuality_16 = 11,
        ImageQuality_17 = 12,
        ImageQuality_18 = 13,
        ImageQuality_19 = 14,
        ImageQuality_20 = 15,
    };
}
namespace app::ropeway::gui::RogueCountDownBehavior {
    enum class CountType : int64_t {
        Invalid = 0xFFFFFFFF,
        CountUp = 0,
        CountDown = 1,
    };
}
namespace via::behaviortree::action::Trace {
    enum class TraceType : int64_t {
        Info = 0,
        Warning = 1,
        ERROR_ = 2,
    };
}
namespace via::detail::atomic {
    enum class SizeClassifier : int64_t {
        Default = 0,
        Native = 1,
    };
}
namespace via::effect::script::EffectManager {
    enum class LODLevel : int64_t {
        None = 0,
        Level01 = 1,
        Level02 = 2,
        Level03 = 3,
        Level04 = 4,
        Level05 = 5,
        Level06 = 6,
        Level07 = 7,
    };
}
namespace via::behaviortree::action {
    enum class DataSetOn : int64_t {
        Start = 0,
        Update = 1,
        NodeEndNotified = 2,
        End = 3,
    };
}
namespace app::ropeway::gamemastering::FsmStateManager {
    enum class RequestType : int64_t {
        SetState = 0,
        CheckCondition = 1,
    };
}
namespace app::ropeway::enemy::em6000::userdata::Em6000ParamUserData::ActionParam {
    enum class ATTRIBUTE : int64_t {
        EyeHiddenDisable = 0,
        GroundDisable = 1,
    };
}
namespace via::motion::JointInfo {
    enum class Type : int64_t {
        Joint = 0,
        Append = 1,
        Extra = 2,
        Locator = 3,
    };
}
namespace app::ropeway::gamemastering::SaveDataManager {
    enum class WarningState : int64_t {
        NONE = 0,
        AUTO_SAVE_ICON = 1,
        GENERAL_DIALOG = 2,
    };
}
namespace app::ropeway::gamemastering::RogueGimmickManager {
    enum class State : int64_t {
        INVALID = 0xFFFFFFFF,
        INITIALIZE = 0,
        WAIT_TO_INGAME = 1,
        INITIALIZE_TO_INGAME = 2,
        ENABLE = 3,
    };
}
namespace app::ropeway::survivor::npc::NpcOrderDefine {
    enum class ThinkCommandKind : int64_t {
        SetIdle = 0,
        SetFollowPlayer = 1,
        Invalid = 2,
    };
}
namespace app::ropeway::enemy::EnemyRelocationPointInfo {
    enum class NODE_TYPE : int64_t {
        General = 0,
        Zombie = 1,
        Licker = 2,
        GAdult = 3,
        Tyrant = 4,
    };
}
namespace via {
    enum class UserState : int64_t {
        Invalid = 65535,
        LogIN_ = 0,
        Logout = 1,
        Max = 2,
    };
}
namespace via::motion::IkLeg {
    enum class RayCastSkipOption : int64_t {
        None = 0,
        DIV2 = 2,
        DIV4 = 4,
    };
}
namespace app::ropeway::gimmick::option::TextMessageSettings::TextMessageInfo::CoordInfo {
    enum class SpecifyType : int64_t {
        GameObjectRef = 0,
        Manual = 1,
    };
}
namespace app::ropeway::OptionManager {
    enum class ControllerHoldButtonType : int64_t {
        TYPE_A = 0,
        TYPE_B = 1,
    };
}
namespace app::ropeway::enemy::em6200::MotionPattern {
    enum class WalkStart : int64_t {
        Front = 0,
        FrontL = 1,
        FrontR = 2,
        BackL = 3,
        BackR = 4,
    };
}
namespace app::ropeway::gimmick::option::EnemyControlSettings::EnemyControlParam {
    enum class SetTargetType : int64_t {
        Player = 0,
        GameObject = 1,
        Position = 2,
        Clear = 3,
    };
}
namespace app::ropeway::enemy::em7100 {
    enum class EyeType : int64_t {
        EyeType_Shoulder = 0,
        EyeType_BackEye = 1,
    };
}
namespace app::ropeway::EnemyGimmickConfiscateController {
    enum class DOOR_OPEN_TYPE : int64_t {
        SinglePush = 0,
        SinglePull = 1,
        DoublePush = 2,
        DoublePull = 3,
    };
}
namespace app::ropeway::enemy::em6200::userdata::Em6200ParamUserData::AttackParam {
    enum class ATTRIBUTE : int64_t {
        IgnoreCameTarget = 0,
        IgnoreGoAwayTarget = 1,
        LockOnOnly = 2,
        DownOnly = 3,
        InCameraOnly = 4,
        MirrorRange = 5,
        IgnoreOnStairs = 6,
        CheckSightTime = 7,
    };
}
namespace via::wwise::SetState2 {
    enum class SetTiming : int64_t {
        Start = 0,
        End = 1,
    };
}
namespace via::reflection {
    enum class TypeKind : int64_t {
        Unknown = 0,
        Enum = 1,
        Boolean = 2,
        Int8 = 3,
        Uint8 = 4,
        Int16 = 5,
        Uint16 = 6,
        Int32 = 7,
        Uint32 = 8,
        Int64 = 9,
        Uint64 = 10,
        Single = 11,
        Double = 12,
        C8 = 13,
        C16 = 14,
        Char = 14,
        String = 15,
        Struct = 16,
        Class = 17,
        Num = 18,
    };
}
namespace via::hid::camera {
    enum class PlayStationCameraCaptureWaitFrameType : int64_t {
        OffMode0 = 1,
        OffMode1 = 2,
        OnMode0 = 3,
        OnMode1 = 4,
    };
}
namespace app::ropeway::gimmick::action::Trigger {
    enum class TriggerLockType : int64_t {
        DEFAULT = 0,
        LOCK = 1,
        LOCK_GET_IT = 2,
        NONE = 3,
    };
}
namespace app::ropeway::behaviortree::BehaviorTreeDefine {
    enum class CheckType : int64_t {
        Equal = 0,
        NotEqual = 1,
    };
}
namespace app::ropeway::fsmv2::GroundFixSwitch {
    enum class SwitchType : int64_t {
        ON_to_OFF = 0,
        OFF_to_ON = 1,
    };
}
namespace System::Globalization {
    enum class TimeSpanStyles : int64_t {
        None = 0,
        AssumeNegative = 1,
    };
}
namespace via::motion::script::FootEffectController {
    enum class JointPartsType : int64_t {
        Foot = 0,
        Hand = 1,
    };
}
namespace app::ropeway::enemy::common::fsmv2::action::NotifyActionEnd {
    enum class EndCheckMethod : int64_t {
        MOTION_END = 0,
        BEFORE_MOTION_END = 1,
        INTERPOLATION_END = 2,
        THIS_FRAME = 3,
    };
}
namespace via::network::ranking::ScoreInfoTbl {
    enum class SortMode : int64_t {
        None = 0,
        RankAscend = 1,
        RankDescend = 2,
        ValueAscend = 3,
        ValueDescend = 4,
    };
}
namespace via::graph {
    enum class VertexRemoveOption : int64_t {
        None = 0,
        WithEdge = 1,
    };
}
namespace app::ropeway::gimmick::action::TriggerKey {
    enum class FourWay : int64_t {
        X_PLUS = 0,
        Z_PLUS = 1,
        X_MINUS = 2,
        Z_MINUS = 3,
        NONE = 4,
        MAX = 4,
    };
}
namespace via::motion::tree {
    enum class ParamType : int64_t {
        Bool = 0,
        U8 = 1,
        S8 = 2,
        U16 = 3,
        S16 = 4,
        S32 = 5,
        U32 = 6,
        S64 = 7,
        U64 = 8,
        F32 = 9,
        F64 = 10,
        Str8 = 11,
        Str16 = 12,
        ExtraData = 13,
        Herimite = 14,
        Guid = 15,
        Vec2 = 16,
        Vec3 = 17,
        Vec4 = 18,
        Matrix = 19,
    };
}
namespace app::ropeway::survivor::jack::InterpSetting {
    enum class InterpKind : int64_t {
        None = 0,
        GameObject = 1,
    };
}
