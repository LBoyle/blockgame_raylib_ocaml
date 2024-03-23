open Raylib;
open Constants;

let array_get_opt = (i, arr) =>
  switch (arr[i]) {
  | item => Some(item)
  | exception _exn => None
  };

let index_to_3d_coord = b => (
  float_of_int(b mod chunkSize),
  float_of_int(b / (chunkSize * chunkSize)),
  float_of_int(b / chunkSize mod chunkSize),
);

let index_to_3d_vec = b => {
  let (x, y, z) = index_to_3d_coord(b);
  Vector3.create(x, y, z);
};

let index_to_2d_coord = ci => (
  float_of_int(ci mod worldSizeInChunks * chunkSize),
  float_of_int(ci / worldSizeInChunks * chunkSize),
);

let index_to_2d_vec = ci => {
  let (x, z) = index_to_2d_coord(ci);
  Vector3.create(x, 0., z);
};

// This allows generation into negative negative, but not into
// negative positive, or positive negative, because world is
// 1D transformed into 2D so it fills only half the graph.
//
// I might need to switch to a real 2D system to generate in
// all directions. So I'm disabling it by clamping above zero
let clamp_index_opt = i =>
  switch (i) {
  // | n when n <= - (worldSizeInChunks * worldSizeInChunks) => None
  | n when n < 0 => None
  | n when n >= worldSizeInChunks * worldSizeInChunks => None
  | n => Some(n)
  };

let sqr = n => n * n;

let euclidean_distance = (px, pz, cx, cz) =>
  sqrt(float_of_int(sqr(cx - px) + sqr(cz - pz)));

// Check if the selected chunk is on the other side of the world
let cull_wrapped_active_chunk = (ci: int, px: int, pz: int) =>
  euclidean_distance(px, pz, ci mod worldSizeInChunks, ci / worldSizeInChunks)
  > float_of_int(viewDistanceInChunks)
    ? None : Some(ci);

let get_active_chunks_ids = (pos: Vector3.t) => {
  let (x, z) = (
    (Vector3.x(pos) |> floor |> int_of_float) / chunkSize,
    (Vector3.z(pos) |> floor |> int_of_float) / chunkSize,
  );
  // Player chunk index
  let pidx = x + z * worldSizeInChunks;
  // FIX ME do this programatically using viewDistanceInChunks
  [
    pidx - worldSizeInChunks - worldSizeInChunks,
    pidx - worldSizeInChunks - 1,
    pidx - worldSizeInChunks,
    pidx - worldSizeInChunks + 1,
    pidx - 2,
    pidx - 1,
    pidx,
    pidx + 1,
    pidx + 2,
    pidx + worldSizeInChunks - 1,
    pidx + worldSizeInChunks,
    pidx + worldSizeInChunks + 1,
    pidx + worldSizeInChunks + worldSizeInChunks,
  ]
  |> List.filter_map(ci =>
       switch (clamp_index_opt(ci)) {
       | None => None
       | Some(ci) => cull_wrapped_active_chunk(ci, x, z)
       }
     );
};
