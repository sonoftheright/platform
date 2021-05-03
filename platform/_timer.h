void p_timer_start(platform_api *api, int index) {
  // find the first open timer slot:
  // int i;
  // for(i = 0; i < P_MAX_TIMERS; i++){
  //   if(api->system)
  // }

  if(index > (int)(sizeof(api->system.timers) / sizeof(p_timer))) {
    p_log("index too large for timers pool.");
    return;
  }

  p_timer found = api->system.timers[index];
  unsigned long long current_ns;
  _p_check_system_clock(&current_ns);
  int timestamp = current_ns;

  if(found.already_started == true) {
    p_log("we've already started this timer somewhere else.");
    return;
  } else {
    api->system.timers[index].startTime = timestamp;
    api->system.timers[index].already_started = true;
  }
}

int p_timer_end(platform_api *api, int index) {
  if(index > (int)(sizeof(api->system.timers) / sizeof(p_timer))) {
    p_log("index too large for timers pool.");
    return -1;
  }

  p_timer found = api->system.timers[index];
  unsigned long long current_ns;
  _p_check_system_clock(&current_ns);
  int timestamp = current_ns;

  if(found.already_started == false) {
    p_log("we haven't started this timer yet.");
    return -1;
  } else {
    api->system.timers[index].endTime = timestamp;
    api->system.timers[index].delta = timestamp - api->system.timers[index].startTime;
    api->system.timers[index].already_started = false;
    return api->system.timers[index].delta;
  }
}
