bool p_toggle_mouse_visible() {
  static bool is_hidden = false;

  /*
  This function sets an internal display counter that determines whether the
  cursor should be displayed. The cursor is displayed only if the display count
  is greater than or equal to 0. If a mouse is installed, the initial display
  count is 0. If no mouse is installed, the display count is –1.
  */

#ifdef _WIN32
  if(is_hidden) {
    // show the cursor:
    ShowCursor(true);
    is_hidden = false;
  } else {
    ShowCursor(false);
    is_hidden = true;
  }
#elif __APPLE__
if(is_hidden) {
  // show the cursor:
  objc_msgSend_id((id)objc_getClass("NSCursor"), sel_registerName("unhide"));
  is_hidden = false;
} else {
  // each "hide" event must have a matching "unhide" to show the cursor again.
  // if we run this twice, and "unhide" only once, it will still be hidden.
  // hide the cursor:
  // [NSCursor hide];
  objc_msgSend_id((id)objc_getClass("NSCursor"), sel_registerName("hide"));
  is_hidden = true;
}
#endif

return is_hidden;
}

void p_mouselock(platform_api *api){
  api->window.prev_mouse_lock = api->window.mouse_lock;

  if(!api->window.mouse_lock){
    bool mouse_lock = p_toggle_mouse_visible();
    api->window.mouse_lock = mouse_lock;
  }
}

void p_handle_mouselock_toggle(platform_api *api, int key){
  bool mouse_lock;

  api->window.prev_mouse_lock = api->window.mouse_lock;

  if (p_key_down(api, key)) {
    p_log("toggling mouselock\n");
    mouse_lock = p_toggle_mouse_visible();
    // setting this also makes sure the mouse stays centered, in _p_handle_mouse:
    api->window.mouse_lock = mouse_lock;
  }
}

void p_update_camera_3d(platform_api *api, int forward, int back, int right, int left){

  static float camera_horizontal_angle;
  static float camera_vertical_angle;
  static float camera_forward_delta[3];
  static float camera_right_delta[3];
  float vertical_clamp_degrees = 90;
  float horizontal_wrap_degrees = 720;

  // This block will reset the mouse input angles as soon as the mouse lock toggles on,
  // so we don't count the initial mouse centering movement as "input"
  bool just_toggled_on = api->window.mouse_lock && !api->window.prev_mouse_lock;
  if(just_toggled_on){
    p_log("just_toggled on is based on current: %i, prev: %i\n", api->window.mouse_lock, api->window.prev_mouse_lock);
  }

  int mouse_delta_x = just_toggled_on ? 0 : api->mouse.delta_position[0];
  int mouse_delta_y = just_toggled_on ? 0 : api->mouse.delta_position[1];


  // if(){
  //   // p_log("we just turned on mouselock!\n");
  //   // p_log("angles were: %f, %f\n", camera_horizontal_angle, camera_vertical_angle);
  //   // camera_horizontal_angle = 3.14;
  //   // camera_vertical_angle = 0.0;
  // }

  // p_log("time delta: %llu, mouse delta: %d, hor angle: %f\r", api->time.ns_smoothed, (api->mouse.delta_position[0] * -1), camera_horizontal_angle);
  // fflush(stdout);

  // first, figure out the delta of the mouse movement:
  if(mouse_delta_x != 0 || mouse_delta_y != 0){
    // @TODO multiply by -1 at the end for "normal" controls, but we should be checking an "invert controls" flag, probably
    camera_horizontal_angle += api->camera.sensitivity * (float)(api->mouse.delta_position[0] * -1);
    camera_vertical_angle += api->camera.sensitivity * (float)(api->mouse.delta_position[1] * -1);

    float vertical_clamp_radians = p_to_radians(vertical_clamp_degrees);
    float horizontal_wrap_radians = p_to_radians(horizontal_wrap_degrees);

    if(camera_vertical_angle > vertical_clamp_radians) {
      camera_vertical_angle = vertical_clamp_radians;
    }
    if(camera_vertical_angle < (-1 * vertical_clamp_radians)) {
      camera_vertical_angle = -1 * vertical_clamp_radians;
    }

    // if we go over 720 degrees, subtract so we stay at a reasonable angle...
    if(camera_horizontal_angle > horizontal_wrap_radians || camera_horizontal_angle < (horizontal_wrap_radians * -1.0)) {
      // if we've somehow got a multiple of 720, make sure we're subtracting as much as we can:
      int multiplier = floor(camera_horizontal_angle / horizontal_wrap_radians);
      camera_horizontal_angle -= (horizontal_wrap_radians * multiplier);
    }

    // p_log("hor,ver ang: [%f, %f] ", camera_horizontal_angle, camera_vertical_angle);

    // Then, set up the camera right, forward, up vectors:
    api->camera.camera_forward[0] = cos(camera_vertical_angle) * sin(camera_horizontal_angle);
    api->camera.camera_forward[1] = sin(camera_vertical_angle);
    api->camera.camera_forward[2] = cos(camera_vertical_angle) * cos(camera_horizontal_angle);

    api->camera.camera_right[0] = sin(camera_horizontal_angle - 3.141592f / 2.0f);
    api->camera.camera_right[1] = 0;
    api->camera.camera_right[2] = cos(camera_horizontal_angle - 3.141592f / 2.0f);

    p_vec3_norm(api->camera.camera_right, api->camera.camera_right);
    p_vec3_norm(api->camera.camera_forward, api->camera.camera_forward);

    // "up" vector is the cross product of right and forward vectors.
    // (orthogonal to right & forward)
    p_vec3_mul_cross(api->camera.camera_up, api->camera.camera_right, api->camera.camera_forward);

    // set up the deltas for camera movement:
    if(api->time.ns_delta <= 0){
      p_log("ns delta is 0!\n");
    }

    camera_forward_delta[0] = api->camera.camera_forward[0] * api->time.ns_delta * api->camera.move_speed;
    camera_forward_delta[1] = api->camera.camera_forward[1] * api->time.ns_delta * api->camera.move_speed;
    camera_forward_delta[2] = api->camera.camera_forward[2] * api->time.ns_delta * api->camera.move_speed;

    camera_right_delta[0] = api->camera.camera_right[0] * api->time.ns_delta * api->camera.move_speed;
    camera_right_delta[1] = api->camera.camera_right[1] * api->time.ns_delta * api->camera.move_speed;
    camera_right_delta[2] = api->camera.camera_right[2] * api->time.ns_delta * api->camera.move_speed;
  }

  // p_log("hor_angle is %f, delta_position is [%i, %i] ", camera_horizontal_angle, api->mouse.delta_position[0], api->mouse.delta_position[1]);

  if(p_key_held(api, forward)) {
    // p_log("moving forward delta: [%f, %f, %f]", camera_forward_delta[0], camera_forward_delta[1], camera_forward_delta[2]);
    p_vec3_add(api->camera.location, camera_forward_delta, api->camera.location);
  }
  if(p_key_held(api, back)) {
    // p_log("moving backward");
    // p_log("moving backward delta: [%f, %f, %f]", camera_forward_delta[0], camera_forward_delta[1], camera_forward_delta[2]);
    p_vec3_sub(api->camera.location, api->camera.location, camera_forward_delta);
  }

  if(p_key_held(api, right)) {
    // p_log("moving right");
    p_vec3_add(api->camera.location, camera_right_delta, api->camera.location);
  }
  if(p_key_held(api, left)) {
    // p_log("moving left");
    p_vec3_sub(api->camera.location, api->camera.location, camera_right_delta);
  }

  // finally, create the projection matrix
  p_mat4x4_identity(api->camera.projection_matrix);// start with identity

  if(api->camera.ortho){
    // @NOTE we need to work on the near/far clip for ortho...
    // also, figure out movement in ortho? Seems like we need to handle zooming in/out manually here...
    // possibly even "fake" an ortho camera with a reallly low-fov perspective camera?
    int left = -10 * api->window.aspect,
        right = 10 * api->window.aspect,
        bottom = -10,
        top = 10;
    p_mat4x4_ortho(api->camera.projection_matrix, left, right, bottom, top, api->camera.near_clip, api->camera.far_clip);
  } else {
    if(!api->camera.far_clip){
      p_mat4x4_perspective_infinite(
        api->camera.projection_matrix,
        api->camera.fov_rad,
        api->window.aspect,
        api->camera.near_clip
      );
    } else {
      p_mat4x4_perspective(
        api->camera.projection_matrix,
        api->camera.fov_rad,
        api->window.aspect,
        api->camera.near_clip,
        api->camera.far_clip
      );
    }
  }

  // Now, generate a lookat matrix for "view"
  p_mat4x4_identity(api->camera.view_matrix);// start with identity

  static float look_at[3];


  p_vec3_add(look_at, api->camera.location, api->camera.camera_forward);
  api->camera.fov_rad = p_to_radians(api->camera.fov);

  p_mat4x4_look_at( api->camera.view_matrix, api->camera.location, look_at, api->camera.camera_up);

}

void p_update_camera_2d(platform_api *api) {
  // "up" vector is the cross product of right and forward vectors.
  // (orthogonal to right & forward)
  p_vec3_mul_cross(api->camera.camera_up, api->camera.camera_right, api->camera.camera_forward);

  // Now, generate a lookat matrix for "view"
  p_mat4x4_identity(api->camera.view_matrix);// start with identity
  p_vec3_add(api->camera.look_matrix, api->camera.location, api->camera.camera_forward);
  p_mat4x4_look_at( api->camera.view_matrix, api->camera.location, api->camera.look_matrix, api->camera.camera_up);
  p_mat4x4_scale(api->camera.view_matrix, api->camera.view_matrix, api->camera.scale);
}
