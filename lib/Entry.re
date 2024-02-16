open Raylib;
open Constants;
open Utils;

// When we decide a chunk is "active" we will check the cache for it,
// if missing then we'll generate one and update the cache
// ChunkCache.find (Map.find), will raise Not_found if the chunk is missing

module ChunkCache =
  Map.Make({
    type t = int;
    let compare = compare;
  });

let chunk_cache: ref(ChunkCache.t(array(Block.t))) = ref(ChunkCache.empty);

let get_chunk_at_index = i => {
  switch (ChunkCache.find(i, chunk_cache^)) {
  | exception _exn =>
    World.generate_chunk(i)
    |> (
      chunk => {
        chunk_cache := ChunkCache.add(i, chunk, chunk_cache^);
        chunk;
      }
    )
  | chunk => chunk
  };
};

type state_t = {
  camera: Camera3D.t,
  activeChunks: list(int),
  playerPosition: Vector3.t,
};

let startingPosition = Vector3.create(4., 6., 4.);

let setup = () => {
  init_window(vhWidth, vhHeight, "blockgame");

  disable_cursor();
  set_target_fps(60);

  {
    camera:
      Camera.create(
        startingPosition,
        Vector3.create(0., 3., 0.),
        Vector3.create(0., 3., 0.),
        80.,
        CameraProjection.Perspective,
      ),
    activeChunks: get_active_chunks_ids(startingPosition),
    playerPosition: startingPosition,
  };
};

let draw_offset_block = (offset, b, block) => {
  let position = Vector3.add(index_to_3d_coord(b), offset);
  switch (Block.get_block_colour(block)) {
  | None => ()
  | Some(blockColour) => draw_cube(position, 1., 1., 1., blockColour)
  };
};

let draw_all = state => {
  begin_drawing();
  clear_background(Color.raywhite);
  begin_mode_3d(state.camera);
  List.iter(
    ci => {
      let chunk = get_chunk_at_index(ci);
      Array.iteri(draw_offset_block(index_to_2d_coord(ci)), chunk);
    },
    state.activeChunks,
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
