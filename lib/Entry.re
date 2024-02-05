open Raylib;
open Constants;

// World is a 1d array 2d grid of Chunks
// a Chunk is a 1d array 3d grid of Blocks
// a Block is a variant or symbol
let world =
  Array.make(worldSizeInChunks * worldSizeInChunks, World.generate_chunk());

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

let get_active_chunks_ids = () => {
  ();
  ();
};

type state_t = {
  camera: Camera3D.t,
  activeChunks: list(int),
  playerPosition: Vector3.t,
};

let startingPosition = Vector3.create(4., 2., 4.);

let setup = () => {
  init_window(vhWidth, vhHeight, "blockgame");
  let camera =
    Camera.create(
      startingPosition,
      Vector3.create(0., 3., 0.),
      Vector3.create(0., 3., 0.),
      80.,
      CameraProjection.Perspective,
    );

  disable_cursor();

  set_target_fps(60);
  {camera, activeChunks: [], playerPosition: startingPosition};
};

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
  let state =
    if (window_should_close()) {
      close_window();
      state;
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
        state.playerPosition,
        Vector3.create(
          Vector2.x(get_mouse_delta()) *. 0.5,
          Vector2.y(get_mouse_delta()) *. 0.5,
          0.,
        ),
        0.,
      );
      {
        ...state,
        playerPosition:
          Vector3.create(
            (is_key_down(Key.S) ? 0. : mv) -. (is_key_down(Key.W) ? 0. : mv),
            (is_key_down(Key.A) ? 0. : mv) -. (is_key_down(Key.D) ? 0. : mv),
            0.,
          ),
      };
    };
  draw_all(state);
  loop(state);
};
