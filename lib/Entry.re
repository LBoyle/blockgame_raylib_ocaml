open Raylib;
open Constants;
open Utils;

// World is a 1d array 2d grid of Chunks
// a Chunk is a 1d array 3d grid of Blocks
// a Block is a variant or symbol
let world =
  Array.make(worldSizeInChunks * worldSizeInChunks, World.generate_chunk());

let get_active_chunks_ids = (pos: Vector3.t) => {
  let (x, z) = (
    (Vector3.x(pos) |> floor |> int_of_float) / chunkSize,
    (Vector3.z(pos) |> floor |> int_of_float) / chunkSize,
  );
  let playerChunk = x + z * worldSizeInChunks;
  // let res =
  //   [
  //     List.init(viewDistanceInChunks * 2, i =>
  //       clamp_index_opt(playerChunk - i)
  //     ),
  //     List.init(viewDistanceInChunks * 2, i =>
  //       clamp_index_opt(playerChunk - i * worldSizeInChunks)
  //     ),
  //   ]
  //   |> List.flatten
  //   |> List.filter_map(x => x);
  let res =
    [
      clamp_index_opt(playerChunk - worldSizeInChunks),
      clamp_index_opt(playerChunk - 1),
      clamp_index_opt(playerChunk),
      clamp_index_opt(playerChunk + 1),
      clamp_index_opt(playerChunk + worldSizeInChunks),
    ]
    |> List.filter_map(x => x);
  res;
};

type state_t = {
  camera: Camera3D.t,
  activeChunks: list(int),
  playerPosition: Vector3.t,
  // testModel: Model.t,
};

let startingPosition = Vector3.create(4., 6., 4.);

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
  {
    camera,
    activeChunks: get_active_chunks_ids(startingPosition),
    playerPosition: startingPosition,
    // testModel: load_model_from_mesh(gen_mesh_plane(3., 3., 10, 10)),
  };
};

let draw_offset_block = (offset, b, block) => {
  let position = Vector3.add(index_to_3d_coord(b), offset);
  // draw_cube_wires(position, 1., 1., 1., Color.maroon);
  switch (Block.get_block_colour(block)) {
  | None => ()
  | Some(blockColour) => draw_cube(position, 1., 1., 1., blockColour)
  };
};
let draw_offset_wires = (offset, b, _block) => {
  let position = Vector3.add(index_to_3d_coord(b), offset);
  draw_cube_wires(position, 1., 1., 1., Color.maroon);
};

let sci = 0;
let sbi = 26;
let specialChunk = world[sci];
let specialBlock = specialChunk[sbi];
let (n, e, s, w, a, b) = ChunkMeshGenerator.get_block_neighbor_ids(sbi);

let draw_all = state => {
  begin_drawing();
  clear_background(Color.raywhite);
  begin_mode_3d(state.camera);
  List.iter(
    ci => {
      let chunk = world[ci];
      Array.iteri(draw_offset_block(index_to_2d_coord(ci)), chunk);
    },
    state.activeChunks,
  );

  draw_offset_wires(index_to_2d_coord(sci), sbi, specialBlock);
  draw_offset_wires(index_to_2d_coord(sci), n, specialBlock);
  draw_offset_wires(index_to_2d_coord(sci), e, specialBlock);
  draw_offset_wires(index_to_2d_coord(sci), s, specialBlock);
  draw_offset_wires(index_to_2d_coord(sci), w, specialBlock);
  draw_offset_wires(index_to_2d_coord(sci), a, specialBlock);
  draw_offset_wires(index_to_2d_coord(sci), b, specialBlock);
  // Array.iteri(
  //   (ci, chunk) =>
  //     Array.iteri(draw_offset_wires(index_to_2d_coord(ci)), chunk),
  //   world,
  // );
  // draw_model(
  //   state.testModel,
  //   Vector3.create(10., 4., 10.),
  //   6.,
  //   Color.lightgray,
  // );
  // draw_model_wires(
  //   state.testModel,
  //   Vector3.create(10., 4., 10.),
  //   6.,
  //   Color.maroon,
  // );
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
      let playerMovement =
        Vector3.create(
          (is_key_down(Key.S) ? 0. : mv) -. (is_key_down(Key.W) ? 0. : mv),
          (is_key_down(Key.A) ? 0. : mv) -. (is_key_down(Key.D) ? 0. : mv),
          0.,
        );
      update_camera_pro(
        addr(state.camera),
        playerMovement,
        Vector3.create(
          Vector2.x(get_mouse_delta()) *. 0.5,
          Vector2.y(get_mouse_delta()) *. 0.5,
          0.,
        ),
        0.,
      );
      let playerPosition = Camera3D.position(state.camera);
      {
        ...state,
        activeChunks: get_active_chunks_ids(playerPosition),
        playerPosition,
      };
    };
  draw_all(state);
  loop(state);
};
