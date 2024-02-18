let rand_unit_vec = () => {
  let theta = Random.float(1.) *. 2. *. Float.pi;
  (cos(theta), sin(theta));
};

module NoiseCache =
  Map.Make({
    type t = (float, float);
    let compare = compare;
  });

let gradient_cache_1: ref(NoiseCache.t((float, float))) =
  ref(NoiseCache.empty);

let noise_cache_1: ref(NoiseCache.t(float)) = ref(NoiseCache.empty);

let dot_prod_grid = (x, z, vx, vz) => {
  let d_vec = (x -. vx, z -. vz);
  let g_vec =
    switch (NoiseCache.find((vx, vz), gradient_cache_1^)) {
    | exception _exn =>
      let res = rand_unit_vec();
      gradient_cache_1 := NoiseCache.add((vx, vz), res, gradient_cache_1^);
      res;
    | v => v
    };
  fst(d_vec) *. fst(g_vec) +. snd(d_vec) *. snd(g_vec);
};

let smootherstep = x => x *. x *. x *. (x *. (6. *. x -. 15.) +. 10.);

let interp = (x, a, b) => a +. smootherstep(x) *. (b -. a);

let perlin_noise_2d = (x, z) => {
  let (x, z) = (x /. 10., z /. 10.);
  switch (NoiseCache.find((x, z), noise_cache_1^)) {
  | v => v
  | exception _exn =>
    let xf = floor(x);
    let zf = floor(z);

    let tl = dot_prod_grid(x, z, xf, zf);
    let tr = dot_prod_grid(x, z, xf +. 1., zf);
    let bl = dot_prod_grid(x, z, xf, zf +. 1.);
    let br = dot_prod_grid(x, z, xf +. 1., zf +. 1.);

    let xt = interp(x -. xf, tl, tr);
    let xb = interp(x -. xf, bl, br);
    let v = interp(z -. zf, xt, xb) *. 0.5 +. 0.5;

    noise_cache_1 := NoiseCache.add((x, z), v, noise_cache_1^);
    v;
  };
};
