open Raylib;
open Constants;

type column_t = {
  height: float,
  position: Raylib.Vector3.t,
  color: Raylib.Color.t,
};

// let chunk1 = World.generate_chunk();
// let chunk2 = World.generate_chunk();

let chunks = Array.make(4, World.generate_chunk());

let setup = () => {
  init_window(width, height, "blockgame");
  let camera =
    Camera.create(
      Vector3.create(4.0, 2.0, 4.0),
      Vector3.create(0.0, 1.8, 0.0),
      Vector3.create(0.0, 1.0, 0.0),
      80.0,
      CameraProjection.Perspective,
    );

  // disable_cursor();

  set_target_fps(60);
  camera;
};

let indexTo3dCoord = b =>
  Vector3.create(
    float_of_int(b mod chunkSize),
    float_of_int(b / (chunkSize * chunkSize)),
    float_of_int(b / chunkSize mod chunkSize),
  );

let draw_block = (b, block) => {
  let position = indexTo3dCoord(b);
  draw_cube_wires(position, 1., 1., 1., Color.maroon);
  switch (Block.get_block_colour(block)) {
  | None => ()
  | Some(blockColour) => draw_cube(position, 1., 1., 1., blockColour)
  };
};

let draw_offset_block = (offset, b, block) => {
  let position = Vector3.add(indexTo3dCoord(b), offset);
  draw_cube_wires(position, 1., 1., 1., Color.maroon);
  switch (Block.get_block_colour(block)) {
  | None => ()
  | Some(blockColour) => draw_cube(position, 1., 1., 1., blockColour)
  };
};

let draw_all = camera => {
  begin_drawing();
  clear_background(Color.raywhite);
  begin_mode_3d(camera);
  Array.iteri(
    (ci, chunk) =>
      Array.iteri(
        draw_offset_block(Vector3.create(float_of_int(ci * 16), 0., 0.)),
        chunk,
      ),
    chunks,
  );
  end_mode_3d();
  end_drawing();
};

let rec loop = camera => {
  if (window_should_close()) {
    close_window();
  } else {
    update_camera_pro(
      addr(camera),
      Vector3.create(
        (is_key_down(Key.S) ? 0. : 0.1) -. (is_key_down(Key.W) ? 0. : 0.1),
        (is_key_down(Key.A) ? 0. : 0.1) -. (is_key_down(Key.D) ? 0. : 0.1),
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
  // let blocks =
  //   if (is_key_pressed(Key.G)) {
  //     Array.append(blocks, World.generate_chunk());
  //   } else {
  //     blocks;
  //   };
  draw_all(camera);
  loop(camera);
};
