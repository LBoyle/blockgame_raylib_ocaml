let cs = Constants.chunkSize;

let generate_chunk = _ci => {
  Array.make(cs * cs, Block.Air)
  |> Array.append(Array.make(cs * cs, Block.Grass))
  |> Array.append(Array.make(cs * cs, Block.Dirt));
};
