#pragma once
#include "esphome.h"

// 定义灯光状态枚举
enum LedState {
  LED_OFF,
  LED_MUTE,
  LED_BLUETOOTH,
  LED_TTS,
  LED_LISTEN,
  LED_WIFI_ERROR,
  LED_TEMP_HIGH,
  LED_STANDBY,
  LED_OTA
};

// 统一的灯光控制函数
inline void set_led_state(LedState state) {
  // 如果被用户物理关闭了，或者是特殊的关灯指令，直接执行关灯
  if (id(led_disabled) || state == LED_OFF) {
    auto call = id(status_led).turn_off();
    call.perform();
    return;
  }

  auto call = id(status_led).turn_on();
  
  // 根据传入的状态设置颜色和特效
  switch (state) {
    case LED_MUTE:
      call.set_brightness(0.5);
      call.set_rgb(1.0, 0.0, 0.5); // 紫红色
      call.set_effect("None");
      break;
    case LED_BLUETOOTH:
      call.set_brightness(id(led_brightness_level) / 100.0);
      call.set_rgb(0.0, 0.0, 1.0); // 蓝色
      call.set_effect("Slow Pulse");
      break;
    case LED_TTS:
      call.set_brightness(0.8);
      call.set_effect("Rainbow");
      break;
    case LED_LISTEN:
      call.set_brightness(0.5);
      call.set_rgb(0.0, 1.0, 1.0); // 青色
      call.set_effect("Slow Pulse");
      break;
    case LED_WIFI_ERROR:
      call.set_brightness(0.5);
      call.set_rgb(1.0, 0.0, 0.0); // 红色常亮
      call.set_effect("None");
      break;
    case LED_OTA:
      call.set_brightness(0.5);
      call.set_rgb(1.0, 0.0, 0.0); // 红色呼吸（与 WiFi 掉线的红常亮区分开）
      call.set_effect("Slow Pulse");
      break;
    case LED_TEMP_HIGH:
      call.set_brightness(0.6);
      call.set_rgb(1.0, 0.53, 0.0); // 橙色
      call.set_effect("Slow Pulse");
      break;
    case LED_STANDBY:
    default:
      call.set_brightness(id(led_brightness_level) / 100.0);
      call.set_rgb(0.0, 1.0, 0.0); // 绿色
      call.set_effect("Slow Pulse");
      break;
  }
  call.perform();
}

// 用于自动逻辑的智能刷新函数
// 优先级：静音 > 过热 > 蓝牙 > TTS播报 > 待机
// 注：过热提到蓝牙/TTS之前，避免放歌或播报时吞掉过热提示（铝壳内 18650 + 功放，必须优先保护）
inline void update_led_auto() {
  if (id(led_disabled)) return; // 被强行关闭，直接退出
  
  if (id(is_hw_muted)) {
    set_led_state(LED_MUTE);
  } else if (id(box_temperature).state > 50.0) {
    set_led_state(LED_TEMP_HIGH);
  } else if (id(is_bt_mode)) {
    set_led_state(LED_BLUETOOTH);
  } else if (id(tts_speaker).state == media_player::MEDIA_PLAYER_STATE_PLAYING) {
    set_led_state(LED_TTS);
  } else {
    // 默认待机
    set_led_state(LED_STANDBY);
  }
}