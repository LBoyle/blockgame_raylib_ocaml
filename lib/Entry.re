open Raylib;
open Constants;

type column_t = {
  height: float,
  position: Raylib.Vector3.t,
  color: Raylib.Color.t,
};

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

  disable_cursor();

  set_target_fps(60);
  (camera, World.initial_blocks());
};

let indexTo3dCoord = b =>
  Vector3.create(
    float_of_int(b mod chunkSize),
    float_of_int(b / (chunkSize * chunkSize)),
    float_of_int(b / chunkSize mod chunkSize),
  );

let draw_all = (camera, blocks) => {
  begin_drawing();
  clear_background(Color.raywhite);
  begin_mode_3d(camera);
  Array.iteri(
    (b, block) => {
      let position = indexTo3dCoord(b);
      draw_cube_wires(position, 1., 1., 1., Color.maroon);
      switch (Block.get_block_colour(block)) {
      | None => ()
      | Some(blockColour) => draw_cube(position, 1., 1., 1., blockColour)
      };
    },
    blocks,
  );
  end_mode_3d();
  end_drawing();
};

let rec loop = ((camera, blocks)) => {
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

  draw_all(camera, blocks);
  loop((camera, blocks));
};
