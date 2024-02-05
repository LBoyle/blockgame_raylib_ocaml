open Raylib;
open Constants;

type column_t = {
  height: float,
  position: Raylib.Vector3.t,
  color: Raylib.Color.t,
};

// How do I load and unload chunks?
// let chunks = Array.make(4, World.generate_chunk());

let world =
  Array.make(worldSizeInChunks * worldSizeInChunks, World.generate_chunk());

type state_t = {
  camera: Camera3D.t,
  activeChunks: list(int),
};

let setup = () => {
  init_window(vhWidth, vhHeight, "blockgame");
  let camera =
    Camera.create(
      Vector3.create(4.0, 3.0, 4.0),
      Vector3.create(0.0, 1.8, 0.0),
      Vector3.create(0.0, 1.0, 0.0),
      80.0,
      CameraProjection.Perspective,
    );

  disable_cursor();

  set_target_fps(60);
  {camera, activeChunks: []};
};

let index_to_3d_coord = b =>
  Vector3.create(
    float_of_int(b mod chunkSize),
    float_of_int(b / (chunkSize * chunkSize)),
    float_of_int(b / chunkSize mod chunkSize),
  );

let index_to_2d_coord = ci =>
  Vector3.create(
    float_of_int(ci mod worldSizeInChunks * chunkSize),
    0.,
    float_of_int(ci / worldSizeInChunks * chunkSize),
  );

let draw_offset_block = (offset, b, block) => {
  let position = Vector3.add(index_to_3d_coord(b), offset);
  draw_cube_wires(position, 1., 1., 1., Color.maroon);
  switch (Block.get_block_colour(block)) {
  | None => ()
  | Some(blockColour) => draw_cube(position, 1., 1., 1., blockColour)
  };
};

let draw_all = state => {
  begin_drawing();
  clear_background(Color.raywhite);
  begin_mode_3d(state.camera);
  Array.iteri(
    (ci, chunk) =>
      Array.iteri(draw_offset_block(index_to_2d_coord(ci)), chunk),
    world,
  );
  end_mode_3d();
  end_drawing();
};

let rec loop = state => {
  if (window_should_close()) {
    close_window();
  } else {
    let delta = get_frame_time();
    let moveSpeed =
      if (is_key_down(Key.Left_shift)) {
        moveSpeed *. 2.;
      } else {
        moveSpeed;
      };
    let mv = moveSpeed *. delta;
    update_camera_pro(
      addr(state.camera),
      Vector3.create(
        (is_key_down(Key.S) ? 0. : mv) -. (is_key_down(Key.W) ? 0. : mv),
        (is_key_down(Key.A) ? 0. : mv) -. (is_key_down(Key.D) ? 0. : mv),
        0.0,
      ),
      Vector3.create(
        Vector2.x(get_mouse_delta()) *. 0.5,
        Vector2.y(get_mouse_delta()) *. 0.5,
        0.0,
      ),
      0.,
    );
  };
  draw_all(state);
  loop(state);
};
