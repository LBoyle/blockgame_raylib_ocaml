// open Raylib;

let cs = Constants.chunkSize;
let ch = Constants.chunkHeight;

let genBlock = (ci, (x, y, z)) => {
  let (cx, cz) = Utils.index_to_2d_coord(ci);
  let (mx, mz) = (x +. cx, z +. cz);

  let noise_sample = Noise.perlin_noise_2d(mx, mz) *. float_of_int(ch) +. 1.;

  if (y < noise_sample -. 2.) {
    Block.Stone;
  } else if (y >= float_of_int(ch - 3)) {
    Block.Air;
  } else if (y < noise_sample) {
    Block.Grass;
  } else {
    Block.Air;
  };
};

let generate_chunk = ci => {
  Array.make(cs * cs * ch, Block.Air)
  |> Array.mapi((i, _air) => genBlock(ci, Utils.index_to_3d_coord(i)));
};
