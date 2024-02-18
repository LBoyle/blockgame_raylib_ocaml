open Raylib;
open Constants;
open Utils;

type state_t = {
  camera: Camera3D.t,
  activeChunks: list(int),
  playerPosition: Vector3.t,
};

// Instead of spawing in the middle with an offset
// I should generate correctly in all 4 directions
// let worldSizeOffsetVec =
//   Vector3.create(
//     float_of_int(worldSizeInChunks / 2 * chunkSize),
//     0.,
//     float_of_int(worldSizeInChunks / 2 * chunkSize),
//   );
let worldSizeOffsetVec = Vector3.create(0., 0., 0.);

let startingPosition = Vector3.create(4., 18., 4.);

let setup = () => {
  init_window(vhWidth, vhHeight, "blockgame");

  disable_cursor();
  set_target_fps(60);

  Random.full_init(world_seed_arr);

  {
    camera:
      Camera.create(
        Vector3.add(startingPosition, worldSizeOffsetVec),
        Vector3.create(0., 3., 0.),
        Vector3.create(0., 3., 0.),
        80.,
        CameraProjection.Perspective,
      ),
    activeChunks: get_active_chunks_ids(startingPosition),
    playerPosition: Vector3.add(startingPosition, worldSizeOffsetVec),
  };
};

let draw_chunk_borders = cv => {
  // This chunk border draws in the wrong place relative
  // to the blocks in the chunk, this might bite me in future
  let position =
    Vector3.add(
      cv,
      Vector3.create(
        float_of_int(chunkSize / 2) -. 0.5,
        float_of_int(chunkHeight / 2) -. 0.5,
        float_of_int(chunkSize / 2) -. 0.5,
      ),
    );
  draw_cube_wires(
    position,
    float_of_int(chunkSize),
    float_of_int(chunkHeight),
    float_of_int(chunkSize),
    Color.maroon,
  );
};

let draw_offset_block = (origin, b, block) => {
  let position = Vector3.add(index_to_3d_vec(b), origin);
  switch (Block.get_block_colour(block)) {
  | None => ()
  | Some(blockColour) =>
    draw_cube(position, 1., 1., 1., blockColour);
    draw_cube_wires(position, 1., 1., 1., Color.maroon);
  };
};

let draw_all = state => {
  begin_drawing();
  clear_background(Color.raywhite);
  begin_mode_3d(state.camera);
  List.iter(
    ci => {
      let chunkOrigin = index_to_2d_vec(ci);
      draw_chunk_borders(chunkOrigin);
      // Drawing blocks is no good at scale, gotta do meshes
      Array.iteri(
        draw_offset_block(chunkOrigin),
        World.get_chunk_at_index(ci),
      );
      ();
    },
    state.activeChunks,
  );

  end_mode_3d();

  let printablePosition =
    Vector3.subtract(state.playerPosition, worldSizeOffsetVec);
  draw_text(
    "x, z : ("
    ++ Printf.sprintf("%.2f", Vector3.x(printablePosition))
    ++ ", "
    ++ Printf.sprintf("%.2f", Vector3.z(printablePosition))
    ++ ")",
    20,
    20,
    20,
    Color.black,
  );

  draw_text("fps: " ++ (get_fps() |> string_of_int), 20, 50, 20, Color.black);

  end_drawing();
};

let rec loop = state => {
  let state =
    if (window_should_close()) {
      World.chunk_cache := World.ChunkCache.create(0);
      Chunk.mesh_cache := Chunk.MeshCache.create(0);
      Noise.gradient_cache_1 := Noise.NoiseCache.create(0);
      Noise.noise_cache_1 := Noise.NoiseCache.create(0);
      close_window();
      state;
    } else {
      let delta = get_frame_time();
      let mv =
        (is_key_down(Key.Left_shift) ? moveSpeed *. 2. : moveSpeed) *. delta;
      let playerMovement =
        Vector3.create(
          (is_key_down(Key.S) ? 0. : mv) -. (is_key_down(Key.W) ? 0. : mv),
          (is_key_down(Key.A) ? 0. : mv) -. (is_key_down(Key.D) ? 0. : mv),
          0.,
        );
      update_camera_pro(
        addr(state.camera), // pointer
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
