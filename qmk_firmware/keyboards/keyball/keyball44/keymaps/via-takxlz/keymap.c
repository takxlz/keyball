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
// レイヤー
enum {
  _BASE = 0, // ベース（QWERTY）
  _MOUSE,    // マウス
  _SYM,      // 記号
  _NUM,      // 数字
  _ADJUST,       // 設定
};

// カスタムキーコード
enum custom_keycodes {
    TK_FWRD = SAFE_RANGE, // 進む
    TK_BACK,              // 戻る
    TK_MCTL,              // ミッションコントロール
    TK_COLN,              // コロン、セミコロン
    TK_PLAS,              // プラス、アスタリスク
    TK_SLEP,              // スリープ
};

// タップダンス
#ifdef TAP_DANCE_ENABLE
// TDキーコード定義
enum {
  TD_LANG = 0, // 言語切替  ※使用していない
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
    layer_on(_SYM);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_on(_SYM);
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
    layer_off(_SYM);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_off(_SYM);
  }
  td_state = 0;
}

// [TD_JPNS]
void td_jpns_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
  if (td_state == SINGLE_HOLD) {
    layer_on(_NUM);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_on(_NUM);
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
    layer_off(_NUM);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_off(_NUM);
  }
  td_state = 0;
}

// [TD_MBTN]
void td_mbtn_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
  // マウスレイヤー
  if (td_state == SINGLE_TAP_HOLD) {
    layer_on(_MOUSE);
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
    layer_off(_MOUSE);
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
// ベースレイヤー
const uint16_t PROGMEM lh_base[] = {
  TD(TD_TBES), KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,
  KC_LCTL,     KC_A,    KC_S,    KC_D,    KC_F,    KC_G,
  KC_LSFT,     KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,
               KC_LGUI, KC_LALT, TD(TD_ENNS), KC_SPC, TD(TD_MBTN)
};
const uint16_t PROGMEM rh_base[] = {
  KC_Y,    KC_U,    KC_I,     KC_O,     KC_P,              KC_BSPC,
  KC_H,    KC_J,    KC_K,     KC_L,     KC_SCLN /* ;: */,  KC_QUOT /* '" */,
  KC_N,    KC_M,    KC_COMM,  KC_DOT,   KC_SLSH /* /? */,  KC_MINS /* -_ */,
           KC_ENT,  TD(TD_JPNS), XXXXXXX, XXXXXXX, KC_ESC
};

// マウスレイヤー
const uint16_t PROGMEM lh_mouse[] = {
  _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______,
           _______, _______, _______, _______, _______
};
const uint16_t PROGMEM rh_mouse[] = {
  _______, _______, _______, _______, _______, _______,
  KC_BTN4, KC_BTN1, KC_BTN2, KC_BTN5, _______, _______,
  _______, _______, _______, _______, _______, _______,
           _______, _______, XXXXXXX, XXXXXXX, _______
};

// 記号レイヤー
const uint16_t PROGMEM lh_sym[] = {
  _______, S(KC_1), S(KC_2), S(KC_3), S(KC_4), S(KC_5),
  _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______,
           _______, _______, _______, _______, _______
};
const uint16_t PROGMEM rh_sym[] = {
  S(KC_6),   S(KC_7),    S(KC_8),   S(KC_9),    S(KC_0),    KC_DEL,
  KC_GRV,    KC_MINS,    KC_EQL,    KC_LBRC,    KC_RBRC,    KC_BSLS,
  S(KC_GRV), S(KC_MINS), S(KC_EQL), S(KC_LBRC), S(KC_RBRC), S(KC_BSLS),
           _______, MO(_ADJUST), XXXXXXX, XXXXXXX, _______
};

// 数字レイヤー
const uint16_t PROGMEM lh_num[] = {
  _______, KC_1,  KC_2,  KC_3,  KC_4,  KC_5,
  _______, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5,
  _______, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10,
           _______, _______, MO(_ADJUST), _______, _______
};
const uint16_t PROGMEM rh_num[] = {
  KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_DEL,
  KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______,
  KC_HOME, KC_PGDN, KC_PGUP, KC_END,  KC_F11,  KC_F12,
           _______, _______, XXXXXXX, XXXXXXX, _______
};

// 設定レイヤー
const uint16_t PROGMEM lh_adjust[] = {
  _______, TK_SLEP, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______,
           _______, _______, _______, _______, _______
};
const uint16_t PROGMEM rh_adjust[] = {
  _______, _______, _______, _______, _______, KC_DEL,
  KC_LALT, KC_LGUI, KC_PSCR, _______, _______, _______,
  _______, _______, _______, _______, _______, _______,
           _______, _______, XXXXXXX, XXXXXXX, _______
};

// キーマップ本体
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE] = LAYOUT_universal(
    lh_base[0], lh_base[1], lh_base[2], lh_base[3], lh_base[4], lh_base[5],
    rh_base[0], rh_base[1], rh_base[2], rh_base[3], rh_base[4], rh_base[5],
    lh_base[6], lh_base[7], lh_base[8], lh_base[9], lh_base[10], lh_base[11],
    rh_base[6], rh_base[7], rh_base[8], rh_base[9], rh_base[10], rh_base[11],
    lh_base[12], lh_base[13], lh_base[14], lh_base[15], lh_base[16], lh_base[17],
    rh_base[12], rh_base[13], rh_base[14], rh_base[15], rh_base[16], rh_base[17],
    lh_base[18], lh_base[19], lh_base[20], lh_base[21], lh_base[22],
    rh_base[18], rh_base[19], rh_base[20], rh_base[21], rh_base[22]
  ),
  [_MOUSE] = LAYOUT_universal(
    lh_mouse[0], lh_mouse[1], lh_mouse[2], lh_mouse[3], lh_mouse[4], lh_mouse[5],
    rh_mouse[0], rh_mouse[1], rh_mouse[2], rh_mouse[3], rh_mouse[4], rh_mouse[5],
    lh_mouse[6], lh_mouse[7], lh_mouse[8], lh_mouse[9], lh_mouse[10], lh_mouse[11],
    rh_mouse[6], rh_mouse[7], rh_mouse[8], rh_mouse[9], rh_mouse[10], rh_mouse[11],
    lh_mouse[12], lh_mouse[13], lh_mouse[14], lh_mouse[15], lh_mouse[16], lh_mouse[17],
    rh_mouse[12], rh_mouse[13], rh_mouse[14], rh_mouse[15], rh_mouse[16], rh_mouse[17],
    lh_mouse[18], lh_mouse[19], lh_mouse[20], lh_mouse[21], lh_mouse[22],
    rh_mouse[18], rh_mouse[19], rh_mouse[20], rh_mouse[21], rh_mouse[22]
  ),
  [_SYM] = LAYOUT_universal(
    lh_sym[0], lh_sym[1], lh_sym[2], lh_sym[3], lh_sym[4], lh_sym[5],
    rh_sym[0], rh_sym[1], rh_sym[2], rh_sym[3], rh_sym[4], rh_sym[5],
    lh_sym[6], lh_sym[7], lh_sym[8], lh_sym[9], lh_sym[10], lh_sym[11],
    rh_sym[6], rh_sym[7], rh_sym[8], rh_sym[9], rh_sym[10], rh_sym[11],
    lh_sym[12], lh_sym[13], lh_sym[14], lh_sym[15], lh_sym[16], lh_sym[17],
    rh_sym[12], rh_sym[13], rh_sym[14], rh_sym[15], rh_sym[16], rh_sym[17],
    lh_sym[18], lh_sym[19], lh_sym[20], lh_sym[21], lh_sym[22],
    rh_sym[18], rh_sym[19], rh_sym[20], rh_sym[21], rh_sym[22]
  ),
  [_NUM] = LAYOUT_universal(
    lh_num[0], lh_num[1], lh_num[2], lh_num[3], lh_num[4], lh_num[5],
    rh_num[0], rh_num[1], rh_num[2], rh_num[3], rh_num[4], rh_num[5],
    lh_num[6], lh_num[7], lh_num[8], lh_num[9], lh_num[10], lh_num[11],
    rh_num[6], rh_num[7], rh_num[8], rh_num[9], rh_num[10], rh_num[11],
    lh_num[12], lh_num[13], lh_num[14], lh_num[15], lh_num[16], lh_num[17],
    rh_num[12], rh_num[13], rh_num[14], rh_num[15], rh_num[16], rh_num[17],
    lh_num[18], lh_num[19], lh_num[20], lh_num[21], lh_num[22],
    rh_num[18], rh_num[19], rh_num[20], rh_num[21], rh_num[22]
  ),
  [_ADJUST] = LAYOUT_universal(
    lh_adjust[0], lh_adjust[1], lh_adjust[2], lh_adjust[3], lh_adjust[4], lh_adjust[5],
    rh_adjust[0], rh_adjust[1], rh_adjust[2], rh_adjust[3], rh_adjust[4], rh_adjust[5],
    lh_adjust[6], lh_adjust[7], lh_adjust[8], lh_adjust[9], lh_adjust[10], lh_adjust[11],
    rh_adjust[6], rh_adjust[7], rh_adjust[8], rh_adjust[9], rh_adjust[10], rh_adjust[11],
    lh_adjust[12], lh_adjust[13], lh_adjust[14], lh_adjust[15], lh_adjust[16], lh_adjust[17],
    rh_adjust[12], rh_adjust[13], rh_adjust[14], rh_adjust[15], rh_adjust[16], rh_adjust[17],
    lh_adjust[18], lh_adjust[19], lh_adjust[20], lh_adjust[21], lh_adjust[22],
    rh_adjust[18], rh_adjust[19], rh_adjust[20], rh_adjust[21], rh_adjust[22]
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
  if (keycode == TK_SLEP && record -> event.pressed) {
    tap_code16(LALT(LGUI(KC_EJCT)));
    // register_code(KC_LALT);
    // register_code(KC_LGUI);
    // tap_code(KC_EJCT);
    // unregister_code(KC_LGUI);
    // unregister_code(KC_LALT);
    return false;
  }
  // コロン、セミコロン
  if (keycode == TK_COLN && record -> event.pressed) {
    bool shift = get_mods() & MOD_MASK_SHIFT;
    if (shift) {
      uint8_t mods = get_mods();  // 現在のModifierの状態を保存
      del_mods(MOD_MASK_SHIFT);   // Shiftを外す
      tap_code(KC_QUOT);          // コロン（del_modsによってShiftを外してアスタリスクからコロンにしている）
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