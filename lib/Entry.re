open Raylib;
open Constants;
open Utils;

// When we decide a chunk is "active" we will check the cache for it,
// if missing then we'll generate one and update the cache
// ChunkCache.find (Map.find), will raise Not_found if the chunk is missing

// So I'm using chunk indexes for 2d coords at low resolution
// then each chunk has a separate array of blocks with 3d coords
// This is all in a global ref dictionary with the chunk index as the key

// An alternative might be a flat Bigarray Int8U or Int16U with offsets
// but I'm not so confident converting an index to a 4d coord
// However this would provide some very performant ways to do things
// like the slicing and whatnot

// I think like this, chunks can be quite tall, 16*16*256 = 65536 blocks
// If I have max int chunks, sqrt of that is world size
// Player coordinates is a double
// I think that's quite a lot larger than minecraft

// In 64bit OCaml the maximums are:
// Int: 4611686018427387903 4.6 quintillion
// Float: 1.79769313486e+308 won't even have accuracy issues

// sqrt of max int is 2147483648 2.14bn same as max cash in osrs
// The world could be 34359738368 or 34bn blocks along a side
// That would be 3.022314549e+23 blocks in a world

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

// let worldSizeOffsetVec =
//   Vector3.create(
//     float_of_int(worldSizeInChunks / 2 * chunkSize),
//     0.,
//     float_of_int(worldSizeInChunks / 2 * chunkSize),
//   );
let worldSizeOffsetVec = Vector3.create(0., 0., 0.);

let startingPosition = Vector3.create(4., 6., 4.);

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
  | Some(blockColour) => draw_cube(position, 1., 1., 1., blockColour)
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
      Array.iteri(draw_offset_block(chunkOrigin), get_chunk_at_index(ci));
      ();
    },
    state.activeChunks,
  );

  end_mode_3d();

  let printablePosition =
    Vector3.subtract(state.playerPosition, worldSizeOffsetVec);
  let positionText =
    "x, z : ("
    ++ Printf.sprintf("%.2f", Vector3.x(printablePosition))
    ++ ", "
    ++ Printf.sprintf("%.2f", Vector3.z(printablePosition))
    ++ ")";
  draw_text(positionText, 20, 20, 20, Color.black);
  let fps = get_fps() |> string_of_int;
  draw_text("fps: " ++ fps, 20, 50, 20, Color.black);

  end_drawing();
};

let rec loop = state => {
  let state =
    if (window_should_close()) {
      chunk_cache := ChunkCache.empty;
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
