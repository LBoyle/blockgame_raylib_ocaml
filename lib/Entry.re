open Raylib;
open Constants;
open Utils;

type state_t = {
  camera: Camera3D.t,
  activeChunks: list(int),
  playerPosition: Vector3.t,
};

let worldSizeOffsetVec = Vector3.create(0., 0., 0.);

let startingPosition = Vector3.create(4., 18., 4.);

let setup = () => {
  set_config_flags([ConfigFlags.Window_resizable]);
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
  let position =
    Vector3.add(
      cv,
      Vector3.create(
        float_of_int(chunkSize / 2),
        float_of_int(chunkHeight / 2),
        float_of_int(chunkSize / 2),
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

// let draw_offset_block = (origin, b, block) => {
//   let position = Vector3.add(index_to_3d_vec(b), origin);
//   switch (Block.get_block_colour(block)) {
//   | None => ()
//   | Some(blockColour) =>
//     draw_cube(position, 1., 1., 1., blockColour);
//     draw_cube_wires(position, 1., 1., 1., Color.maroon);
//   };
// };

let draw_all = state => {
  begin_drawing();
  clear_background(Color.raywhite);
  begin_mode_3d(state.camera);
  List.iter(
    ci => {
      let chunk_origin = index_to_2d_vec(ci);
      draw_chunk_borders(chunk_origin);
      // Drawing blocks is no good at scale, gotta do meshes
      // Array.iteri(
      //   draw_offset_block(chunk_origin),
      //   World.get_chunk_at_index(ci),
      // );

      switch (Chunk.get_mesh_for_chunk_opt(ci)) {
      | None => ()
      | Some(chunk_mesh) =>
        let chunk_model = load_model_from_mesh(chunk_mesh.mesh);
        draw_model_wires(chunk_model, chunk_origin, 1.0, Color.maroon);
      };
    },
    state.activeChunks,
  );

  draw_cube(Vector3.create(0., 0., 0.), 1., 1., 1., Color.red); // origin
  draw_cube(Vector3.create(2., 0., 0.), 1., 1., 1., Color.green); // x
  draw_cube(Vector3.create(0., 0., 2.), 1., 1., 1., Color.blue); // z
  draw_cube(Vector3.create(0., 2., 0.), 1., 1., 1., Color.brown); // y

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
      World.ChunkCache.reset(World.chunk_cache);
      Noise.NoiseCache.reset(Noise.gradient_cache_1);
      Noise.NoiseCache.reset(Noise.noise_cache_1);
      Chunk.MeshCache.reset(Chunk.mesh_cache);
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
          (is_key_down(Key.Left_control) ? 0. : mv)
          -. (is_key_down(Key.Space) ? 0. : mv),
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
      let updated_active_chunks = get_active_chunks_ids(playerPosition);
      let _ =
        List.iter(
          ci => {
            Chunk.get_mesh_for_chunk_generate(ci)
            |> (
              chunk =>
                if (!chunk.loaded) {
                  // This can cause a segfault
                  // I suspect it's because OCaml GC has cleared
                  // something that I have passed to the C code
                  upload_mesh(addr(chunk.mesh), false);
                  Chunk.MeshCache.replace(
                    Chunk.mesh_cache,
                    ci,
                    {...chunk, loaded: true},
                  );
                }
            )
          },
          updated_active_chunks,
        );
      {...state, activeChunks: updated_active_chunks, playerPosition};
    };
  draw_all(state);
  loop(state);
};
