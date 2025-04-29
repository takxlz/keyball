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
  _NS,       // 数字・記号
  _NSS,      // 数字・記号（サブ）
  _MV,       // 移動
  _FN,       // ファンクション
};

// カスタムキーコード
enum custom_keycodes {
    TK_FWRD = SAFE_RANGE, // 進む
    TK_BACK,              // 戻る
    TK_MCTL,              // ミッションコントロール
    TK_COLN,              // コロン、セミコロン
    TK_PLAS,              // プラス、アスタリスク
};

// タップダンス
#ifdef TAP_DANCE_ENABLE
// TDキーコード定義
enum {
  TD_LANG = 0, // 言語切替  ※使用していない
  TD_ENNS,     // 英数、数字・記号
  TD_JPNS,     // かな、数字・記号
  TD_MBTN,     // マウス
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

// [TD_ENNS]
void td_enns_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
 if (td_state == SINGLE_HOLD) {
    layer_on(_NS);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_on(_NSS);
  } else {
    if (detected_host_os() == OS_WINDOWS) {
      tap_code(KC_LNG2);
      tap_code(KC_F13);
    } else {
      tap_code(KC_LNG2);
    }
  }
}
void td_enns_reset(tap_dance_state_t *state, void *user_data) {
  if (td_state == SINGLE_HOLD) {
    layer_off(_NS);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_off(_NSS);
  }
  td_state = 0;
}

// [TD_JPNS]
void td_jpns_finished(tap_dance_state_t *state, void *user_data) {
  td_state = cur_dance(state);
  if (td_state == SINGLE_HOLD) {
    layer_on(_NS);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_on(_NSS);
  } else {
    if (detected_host_os() == OS_WINDOWS) {
      tap_code(KC_LNG1);
      tap_code(KC_F14);
    } else {
      tap_code(KC_LNG1);
    }
  }
}
void td_jpns_reset(tap_dance_state_t *state, void *user_data) {
  if (td_state == SINGLE_HOLD) {
    layer_off(_NS);
  } else if (td_state == SINGLE_TAP_HOLD) {
    layer_off(_NSS);
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

// TD割り当て
tap_dance_action_t tap_dance_actions[] = {
    [TD_LANG] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lang_finished, td_lang_reset),
    [TD_ENNS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_enns_finished, td_enns_reset),
    [TD_JPNS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_jpns_finished, td_jpns_reset),
    [TD_MBTN] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_mbtn_finished, td_mbtn_reset),
};
#endif


/************************************************************ キー割り当て ************************************************************/
// clang-format off
// ベースレイヤー
const uint16_t PROGMEM lh_base[] = {
  KC_ESC,          KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,
  LSFT_T(KC_TAB),  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,
  KC_LCTL,         KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,
                   KC_LALT, KC_LGUI, TD(TD_ENNS), KC_SPC, TD(TD_MBTN)
};
const uint16_t PROGMEM rh_base[] = {
  KC_Y,    KC_U,    KC_I,     KC_O,     KC_P,              KC_BSPC,
  KC_H,    KC_J,    KC_K,     KC_L,     KC_MINS /* -= */,  RSFT_T(KC_ENT),
  KC_N,    KC_M,    KC_COMM,  KC_DOT,   KC_SLSH /* /? */,  KC_INT1 /* \_ */,
           MO(_MV), TD(TD_JPNS), XXXXXXX, XXXXXXX, LT(_MV, KC_BTN1)
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

// 数字・記号レイヤー
const uint16_t PROGMEM lh_ns[] = {
  _______, _______, S(KC_RBRC) /* { */,   S(KC_NUHS) /* } */,   _______,           _______,
  _______, _______, KC_RBRC /* [{ */,     KC_NUHS /* ]} */,     KC_LBRC /* @` */,  _______,
  _______, _______, _______,              _______,              _______,           _______,
           _______, _______, MO(_FN), _______, _______
};
const uint16_t PROGMEM rh_ns[] = {
  _______,            KC_7 /* ' */, KC_8 /* ( */, KC_9 /* ) */, KC_EQL /* ^~ */,  KC_BSPC,
  KC_0,               KC_4 /* $ */, KC_5 /* % */, KC_6 /* & */, KC_MINS /* -= */, TK_COLN /* ;: */,
  TK_PLAS /* +* */,   KC_1 /* ! */, KC_2 /* " */, KC_3 /* # */, KC_SLSH /* /? */, KC_INT1 /* \_ */,
                      _______, MO(_FN), XXXXXXX, XXXXXXX, KC_INT3 /* ¥| */
};

// 数字・記号（サブ）レイヤー
const uint16_t PROGMEM lh_nss[] = {
  _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______,
           _______, _______, MO(_FN), _______, _______
};
const uint16_t PROGMEM rh_nss[] = {
  _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, _______, _______,
           _______, MO(_FN), XXXXXXX, XXXXXXX, _______
};

// 移動レイヤー
const uint16_t PROGMEM lh_mv[] = {
  _______, _______, _______, _______, _______, _______,
  _______, _______, _______, TK_BACK, TK_FWRD, _______,
  _______, _______, _______, _______, _______, _______,
           _______, _______, _______, _______, _______
};
const uint16_t PROGMEM rh_mv[] = {
  KC_PGUP, KC_HOME, KC_END,  KC_PGDN, _______, KC_DEL,
  KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______,
  _______, _______, _______, _______, _______, KC_EJCT,
           _______, _______, XXXXXXX, XXXXXXX, _______
};

// FNレイヤー
const uint16_t PROGMEM lh_fn[] = {
  _______, _______, _______, _______, KBC_RST, _______,
  _______, AML_TO,  KBC_SAVE, _______, TK_MCTL, _______,
  _______, _______, _______, _______, _______, _______,
           _______, _______, _______, _______, _______
};
const uint16_t PROGMEM rh_fn[] = {
  KC_F12, KC_F7, KC_F8, KC_F9, KC_PSCR, KC_DEL,
  KC_F11, KC_F4, KC_F5, KC_F6, _______, _______,
  KC_F10, KC_F1, KC_F2, KC_F3, _______, _______,
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
  [_NS] = LAYOUT_universal(
    lh_ns[0], lh_ns[1], lh_ns[2], lh_ns[3], lh_ns[4], lh_ns[5],
    rh_ns[0], rh_ns[1], rh_ns[2], rh_ns[3], rh_ns[4], rh_ns[5],
    lh_ns[6], lh_ns[7], lh_ns[8], lh_ns[9], lh_ns[10], lh_ns[11],
    rh_ns[6], rh_ns[7], rh_ns[8], rh_ns[9], rh_ns[10], rh_ns[11],
    lh_ns[12], lh_ns[13], lh_ns[14], lh_ns[15], lh_ns[16], lh_ns[17],
    rh_ns[12], rh_ns[13], rh_ns[14], rh_ns[15], rh_ns[16], rh_ns[17],
    lh_ns[18], lh_ns[19], lh_ns[20], lh_ns[21], lh_ns[22],
    rh_ns[18], rh_ns[19], rh_ns[20], rh_ns[21], rh_ns[22]
  ),
  [_NSS] = LAYOUT_universal(
    lh_nss[0], lh_nss[1], lh_nss[2], lh_nss[3], lh_nss[4], lh_nss[5],
    rh_nss[0], rh_nss[1], rh_nss[2], rh_nss[3], rh_nss[4], rh_nss[5],
    lh_nss[6], lh_nss[7], lh_nss[8], lh_nss[9], lh_nss[10], lh_nss[11],
    rh_nss[6], rh_nss[7], rh_nss[8], rh_nss[9], rh_nss[10], rh_nss[11],
    lh_nss[12], lh_nss[13], lh_nss[14], lh_nss[15], lh_nss[16], lh_nss[17],
    rh_nss[12], rh_nss[13], rh_nss[14], rh_nss[15], rh_nss[16], rh_nss[17],
    lh_nss[18], lh_nss[19], lh_nss[20], lh_nss[21], lh_nss[22],
    rh_nss[18], rh_nss[19], rh_nss[20], rh_nss[21], rh_nss[22]
  ),
  [_MV] = LAYOUT_universal(
    lh_mv[0], lh_mv[1], lh_mv[2], lh_mv[3], lh_mv[4], lh_mv[5],
    rh_mv[0], rh_mv[1], rh_mv[2], rh_mv[3], rh_mv[4], rh_mv[5],
    lh_mv[6], lh_mv[7], lh_mv[8], lh_mv[9], lh_mv[10], lh_mv[11],
    rh_mv[6], rh_mv[7], rh_mv[8], rh_mv[9], rh_mv[10], rh_mv[11],
    lh_mv[12], lh_mv[13], lh_mv[14], lh_mv[15], lh_mv[16], lh_mv[17],
    rh_mv[12], rh_mv[13], rh_mv[14], rh_mv[15], rh_mv[16], rh_mv[17],
    lh_mv[18], lh_mv[19], lh_mv[20], lh_mv[21], lh_mv[22],
    rh_mv[18], rh_mv[19], rh_mv[20], rh_mv[21], rh_mv[22]
  ),
  [_FN] = LAYOUT_universal(
    lh_fn[0], lh_fn[1], lh_fn[2], lh_fn[3], lh_fn[4], lh_fn[5],
    rh_fn[0], rh_fn[1], rh_fn[2], rh_fn[3], rh_fn[4], rh_fn[5],
    lh_fn[6], lh_fn[7], lh_fn[8], lh_fn[9], lh_fn[10], lh_fn[11],
    rh_fn[6], rh_fn[7], rh_fn[8], rh_fn[9], rh_fn[10], rh_fn[11],
    lh_fn[12], lh_fn[13], lh_fn[14], lh_fn[15], lh_fn[16], lh_fn[17],
    rh_fn[12], rh_fn[13], rh_fn[14], rh_fn[15], rh_fn[16], rh_fn[17],
    lh_fn[18], lh_fn[19], lh_fn[20], lh_fn[21], lh_fn[22],
    rh_fn[18], rh_fn[19], rh_fn[20], rh_fn[21], rh_fn[22]
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