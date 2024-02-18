open Constants;

let genBlock = (ci, (x, y, z)) => {
  let (cx, cz) = Utils.index_to_2d_coord(ci);
  let (mx, mz) = (x +. cx, z +. cz);

  let noise_sample =
    Noise.perlin_noise_2d(mx, mz) *. float_of_int(chunkHeight);

  if (y < noise_sample -. 2.) {
    Block.Stone;
  } else if (y < noise_sample) {
    Block.Grass;
  } else {
    Block.Air;
  };
};

let generate_chunk = ci => {
  Array.make(chunkSize * chunkSize * chunkHeight, Block.Air)
  |> Array.mapi((i, _air) => genBlock(ci, Utils.index_to_3d_coord(i)));
};
