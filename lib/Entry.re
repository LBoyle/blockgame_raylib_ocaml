open Raylib;

let width = 1920;
let height = 1080;

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

let draw_all = (camera, blocks) => {
  begin_drawing();
  clear_background(Color.raywhite);
  begin_mode_3d(camera);
  Array.iteri(
    (height, ltr) => {
      Array.iteri(
        (i, fwb) => {
          Array.iteri(
            (j, block) => {
              let position =
                Vector3.create(
                  float_of_int(i),
                  float_of_int(height),
                  float_of_int(j),
                );
              switch (Block.get_block_colour(block)) {
              | None => ()
              | Some(blockColour) =>
                draw_cube(position, 2.0, 1., 2.0, blockColour)
              };

              draw_cube_wires(position, 2.0, 1., 2.0, Color.maroon);
              ();
            },
            fwb,
          );
          ();
        },
        ltr,
      )
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
