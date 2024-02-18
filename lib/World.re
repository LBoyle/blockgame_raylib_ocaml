// open Raylib;

let cs = Constants.chunkSize;
let ch = Constants.chunkHeight;

module Noise = {
  let white_noise_2d = (_x, _z) => {
    Random.float(1.);
  };
  let rand_unit_vec = () => {
    let theta = Random.float(1.) *. 2. *. Float.pi;
    Raylib.Vector2.create(cos(theta), sin(theta));
  };
  let perlin_noise_2d = (x, y) => {
    ();
    0.;
  };
};

let genBlock = ((x, y, z)) => {
  let noise_sample = Noise.white_noise_2d(int_of_float(x), int_of_float(z));
  let y_sample = noise_sample *. float_of_int(ch);
  if (y == 0.) {
    Block.Stone;
  } else if (y == float_of_int(ch - 1)) {
    Block.Air;
  } else if (y < y_sample) {
    Block.Grass;
  } else {
    Block.Air;
  };
};

let generate_chunk = _ci => {
  Array.make(cs * cs * ch, Block.Air)
  |> Array.mapi((i, _air) => genBlock(Utils.index_to_3d_coord(i)));
};
