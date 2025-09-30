#include QMK_KEYBOARD_H
#include "quantum.h"
#ifdef OS_DETECTION_ENABLE
  #include "os_detection.h"
#endif

/************************************************************ OLED ************************************************************/
#ifdef OLED_ENABLE
# include "lib/oledkit/oledkit.h"
void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif


/************************************************************ 各種定義 ************************************************************/
// カスタムキーコード
enum custom_keycodes {
    TK_FWRD = SAFE_RANGE, // 進む
    TK_BACK,              // 戻る
    TK_MCTL,              // ミッションコントロール
    TK_SCLN,              // セミコロン、コロン
    TK_PLAS,              // プラス、アスタリスク
    TK_SLEP,              // スリープ
    TK_EISU,              // 英数
    TK_KANA,              // かな
};

// タップダンス
#ifdef TAP_DANCE_ENABLE
// TDキーコード定義
enum {
  TD_LANG = 0, // 言語切替         ※使用していない
  TD_ENNS,     // 英数、数字・記号
  TD_JPNS,     // かな、数字・記号
  TD_MBTN,     // マウス
  TD_TBES    // タブ、エスケープ
};

// TD種別
enum {
  SINGLE_TAP = 1,    // タップ
  SINGLE_HOLD,       // ホールド
  DOUBLE_TAP,        // ダブルタップ
  SINGLE_TAP_HOLD,   // タップ＆ホールド
  DOUBLE_SINGLE_TAP, // appleのようなpp入力とダブルタップを区別
};
#endif

/************************************************************ タップダンス ************************************************************/
#ifdef TAP_DANCE_ENABLE
// TD状態
static int td_state = 0;

// TD種別判定関数
int cur_dance(tap_dance_state_t *state) {
  if (state->count ==1) {
    if (!state->pressed) return SINGLE_TAP;
    else return SINGLE_HOLD;
  }
  else if (state->count == 2) {
    if (state->interrupted) return DOUBLE_SINGLE_TAP;
    else if (state->pressed) return SINGLE_TAP_HOLD;
    else return DOUBLE_TAP;
  }
  else return 9;
}

// [TD_LANG]
/*
void td_lang_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
  // 英数
  if (td_state == SINGLE_TAP || td_state == SINGLE_HOLD) {
    if (detected_host_os() == OS_WINDOWS) {
      tap_code(KC_LNG2);
      tap_code(KC_F13);
    } else {
      tap_code(KC_LNG2);
    }
  // かな
  } else {
    if (detected_host_os() == OS_WINDOWS) {
      tap_code(KC_LNG1);
      tap_code(KC_F14);
    } else {
      tap_code(KC_LNG1);
    }
  }
}
void td_lang_reset(tap_dance_state_t *state, void *user_data) {
  td_state = 0;
}
*/

// [TD_ENNS]
void td_enns_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
 if (td_state == SINGLE_HOLD) {
    layer_on(4);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_on(2);
  } else {
    if (detected_host_os() == OS_WINDOWS) {
      tap_code(KC_LNG2);
      tap_code(KC_F22);
    } else {
      tap_code(KC_LNG2);
    }
  }
}
void td_enns_reset(tap_dance_state_t *state, void *user_data) {
  if (td_state == SINGLE_HOLD) {
    layer_off(4);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_off(2);
  }
  td_state = 0;
}

// [TD_JPNS]
void td_jpns_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
  if (td_state == SINGLE_HOLD) {
    layer_on(2);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_on(2);
  } else {
    if (detected_host_os() == OS_WINDOWS) {
      tap_code(KC_LNG1);
      tap_code(KC_F21);
    } else {
      tap_code(KC_LNG1);
    }
  }
}
void td_jpns_reset(tap_dance_state_t *state, void *user_data) {
  if (td_state == SINGLE_HOLD) {
    layer_off(2);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_off(2);
  }
  td_state = 0;
}

// [TD_MBTN]
void td_mbtn_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
  // マウスレイヤー
  if (td_state == SINGLE_TAP_HOLD) {
    layer_on(1);
  // マウスボタン1
  } else if (td_state == SINGLE_HOLD) {
    register_code(KC_BTN1);
  } else if (td_state == SINGLE_TAP) {
    tap_code(KC_BTN1);
  } else if (td_state == DOUBLE_TAP) {
    tap_code(KC_BTN1);
    tap_code(KC_BTN1);
  }
}
void td_mbtn_reset(tap_dance_state_t *state, void *user_data) {
  // マウスレイヤー
  if (td_state == SINGLE_TAP_HOLD) {
    layer_off(1);
  } else if (td_state == SINGLE_HOLD) {
    unregister_code(KC_BTN1);
  }
  td_state = 0;
}

// [TD_TBES]
void td_tbes_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
  if (td_state == SINGLE_TAP || td_state == DOUBLE_TAP) {
    tap_code(KC_TAB);
  } else {
    tap_code(KC_ESC);
  }
}
void td_tbes_reset(tap_dance_state_t *state, void *user_data) {
  td_state = 0;
}

// TD割り当て
tap_dance_action_t tap_dance_actions[] = {
    // [TD_LANG] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lang_finished, td_lang_reset),
    [TD_ENNS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_enns_finished, td_enns_reset),
    [TD_JPNS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_jpns_finished, td_jpns_reset),
    [TD_MBTN] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_mbtn_finished, td_mbtn_reset),
    [TD_TBES] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_tbes_finished, td_tbes_reset),
};
#endif


/************************************************************ キー割り当て ************************************************************/
// clang-format off
// ショートカット
#define SC_MTGUI MT(MOD_LGUI, KC_MINS)
#define SC_MTALT MT(MOD_LALT, KC_SLSH)
#define SC_LT3ENT LT(3, KC_ENT)
// #define SC_MTCTL MT(MOD_LCTL, KC_ENT)

// キーマップ
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

// ベース
  [0] = LAYOUT_universal(
// ,-----------------------------------------------------------------------------------.         .-----------------------------------------------------------------------------------.
    TD(TD_TBES)  , KC_Q        , KC_W        , KC_E        , KC_R        , KC_T        ,          KC_Y         , KC_U        , KC_I        , KC_O        , KC_P        , KC_BSPC     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
    KC_LCTL      , KC_A        , KC_S        , KC_D        , KC_F        , KC_G        ,          KC_H         , KC_J        , KC_K        , KC_L        , SC_MTGUI    , KC_QUOT     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
    KC_LSFT      , KC_Z        , KC_X        , KC_C        , LT(3, KC_V) , KC_B        ,          KC_N         , KC_M        , KC_COMM     , KC_DOT      , SC_MTALT    , KC_SCLN     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
                   KC_LGUI     , KC_LALT     , TD(TD_ENNS) , KC_SPC      , TD(TD_MBTN) ,          SC_LT3ENT    , TD(TD_JPNS) , XXXXXXX     , XXXXXXX     , KC_ESC
// ,              ---------------------------------------------------------------------.         .----------------------------------------------------------------------             .
  ),

// マウス
  [1] = LAYOUT_universal(
// ,-----------------------------------------------------------------------------------.         .-----------------------------------------------------------------------------------.
    _______      , _______     , _______     , _______     , _______     , _______     ,          _______      , _______     , _______     , _______     , _______     , _______     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
    _______      , _______     , _______     , _______     , _______     , _______     ,          KC_BTN4      , KC_BTN1     , KC_BTN2     , KC_BTN5     , _______     , _______     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
    _______      , _______     , _______     , _______     , _______     , _______     ,          _______      , _______     , _______     , _______     , _______     , _______     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
                   _______     , _______     , _______     , _______     , _______     ,          _______      , _______     , XXXXXXX     , XXXXXXX     , _______
// ,              ---------------------------------------------------------------------.         .----------------------------------------------------------------------             .
  ),

// 数字、記号
  [2] = LAYOUT_universal(
// ,-----------------------------------------------------------------------------------.         .-----------------------------------------------------------------------------------.
    _______      , S(KC_LBRC)  , KC_7        , KC_8        , KC_9        , KC_EQL      ,          S(KC_QUOT)   , KC_SLSH     , S(KC_8)     , S(KC_9)     , _______     , _______     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
    _______      , KC_LBRC     , KC_4        , KC_5        , KC_6        , S(KC_EQL)   ,          S(KC_SCLN)   , KC_MINS     , KC_RBRC     , KC_NUHS     , KC_INT1     , _______     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
    _______      , _______     , KC_1        , KC_2        , KC_3        , KC_0        ,          S(KC_MINS)   , S(KC_INT1)  , S(KC_RBRC)  , S(KC_NUHS)  , S(KC_INT3)  , KC_RSFT     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
                   _______     , _______     , KC_LSFT     , _______     , _______     ,          _______      , _______     , XXXXXXX     , XXXXXXX     , _______
// ,              ---------------------------------------------------------------------.         .----------------------------------------------------------------------             .
  ),

  // 数字裏記号、移動
  [3] = LAYOUT_universal(
// ,-----------------------------------------------------------------------------------.         .-----------------------------------------------------------------------------------.
    _______      , _______     , S(KC_7)     , S(KC_8)     , S(KC_9)     , _______     ,          KC_HOME      , KC_PGDN     , KC_PGUP     , KC_END      , KC_PSCR     , _______     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
    _______      , _______     , S(KC_4)     , S(KC_5)     , S(KC_6)     , _______     ,          KC_LEFT      , KC_DOWN     , KC_UP       , KC_RGHT     , _______     , _______     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
    _______      , _______     , S(KC_1)     , S(KC_2)     , S(KC_3)     , _______     ,          _______      , _______     , _______     , _______     , _______     , _______     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
                   _______     , _______     , _______     , _______     , _______     ,          _______      , _______     , XXXXXXX     , XXXXXXX     , _______
// ,              ---------------------------------------------------------------------.         .----------------------------------------------------------------------             .
  ),

// ファンクション、設定
  [4] = LAYOUT_universal(
// ,-----------------------------------------------------------------------------------.         .-----------------------------------------------------------------------------------.
    _______      , _______     , KC_F7       , KC_F8       , KC_F9       , KC_F10      ,          _______      , _______     , _______     , _______     , _______     , KC_DEL      ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
    _______      , _______     , KC_F4       , KC_F5       , KC_F6       , KC_F11      ,          KC_BSPC      , TK_FWRD     , TK_BACK     , _______     , _______     , _______     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
    _______      , _______     , KC_F1       , KC_F2       , KC_F3       , KC_F12      ,          _______      , TK_MCTL     , _______     , _______     , _______     , _______     ,
// ,-------------+-------------+-------------+-------------+-------------+-------------.         .-------------+-------------+-------------+-------------+-------------+-------------.
                   _______     , _______     , _______     , _______     , _______     ,          _______      , _______     , XXXXXXX     , XXXXXXX     , TK_SLEP
// ,              ---------------------------------------------------------------------.         .----------------------------------------------------------------------             .
  )
};
// clang-format on


/************************************************************ カスタムキー ************************************************************/
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  // 進む
  if (keycode == TK_FWRD && record -> event.pressed) {
    if (detected_host_os() == OS_WINDOWS) {
      register_code(KC_LALT);
      tap_code(KC_RGHT);
      unregister_code(KC_LALT);
    } else {
      register_code(KC_LGUI);
      tap_code(KC_RGHT);
      unregister_code(KC_LGUI);
    }
    return false;
  }
  // 戻る
  if (keycode == TK_BACK && record -> event.pressed) {
    if (detected_host_os() == OS_WINDOWS) {
      register_code(KC_LALT);
      tap_code(KC_LEFT);
      unregister_code(KC_LALT);
    } else {
      register_code(KC_LGUI);
      tap_code(KC_LEFT);
      unregister_code(KC_LGUI);
    }
    return false;
  }
  // ミッションコントロール
  if (keycode == TK_MCTL && record -> event.pressed) {
    if (detected_host_os() == OS_WINDOWS) {
      register_code(KC_LCTL);
      register_code(KC_LALT);
      tap_code(KC_TAB);
      unregister_code(KC_LALT);
      unregister_code(KC_LCTL);
    } else {
      register_code(KC_LCTL);
      tap_code(KC_UP);
      unregister_code(KC_LCTL);
    }
    return false;
  }
  // 英数
 if (keycode == TK_EISU && record -> event.pressed) {
    if (detected_host_os() == OS_WINDOWS) {
      tap_code(KC_LNG2);
      tap_code(KC_F22);
    } else {
      tap_code(KC_LNG2);
    }
    return false;
  }
  // かな
 if (keycode == TK_KANA && record -> event.pressed) {
    if (detected_host_os() == OS_WINDOWS) {
      tap_code(KC_LNG1);
      tap_code(KC_F21);
    } else {
      tap_code(KC_LNG1);
    }
    return false;
  }
  // スリープ
  if (keycode == TK_SLEP && record -> event.pressed) {
    tap_code16(LALT(LGUI(KC_EJCT)));
    // register_code(KC_LALT);
    // register_code(KC_LGUI);
    // tap_code(KC_EJCT);
    // unregister_code(KC_LGUI);
    // unregister_code(KC_LALT);
    return false;
  }
  // セミコロン、コロン
  if (keycode == TK_SCLN && record -> event.pressed) {
    bool shift = get_mods() & MOD_MASK_SHIFT;
    if (shift) {
      uint8_t mods = get_mods();  // 現在のModifierの状態を保存
      del_mods(MOD_MASK_SHIFT);   // Shiftを外す
      tap_code(KC_QUOT);          // コロン（del_modsによってShiftを外す）
      set_mods(mods);             // 元のModifier状態に戻す
    } else {
      tap_code(KC_SCLN);          // セミコロン
    }
    return false;
  }
  // プラス、アスタリスク
  if (keycode == TK_PLAS && record -> event.pressed) {
    bool shift = get_mods() & MOD_MASK_SHIFT;
    if (shift) {
      tap_code(KC_QUOT);          // アスタリスク
    } else {
      add_mods(MOD_MASK_SHIFT);   // Shiftを追加する
      tap_code(KC_SCLN);          // プラス（add_modsによってShiftを追加してセミコロンからプラスにしている）
      del_mods(MOD_MASK_SHIFT);   // Shiftを外す
    }
    return false;
  }
  return true;
};


layer_state_t layer_state_set_user(layer_state_t state) {
    // スクロールモード
    // NOTE: マウスレイヤーよりも後ろのレイヤーを設定すること
    // オートマウスとスクロールモードのレイヤーが同時に有効になると後ろのレイヤーが優先させるため、スクロールモードが前にあるとスクロールされない
    keyball_set_scroll_mode(get_highest_layer(state) >= 2);
    return state;
}