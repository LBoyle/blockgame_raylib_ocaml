open Constants;
open Raylib;

// Because blocks are a 1d array this will select
// blocks that are wrapped, on the other side of the chunk
let get_block_neighbor_ids = b => {
  (
    b - 1,
    b + 1,
    b - chunkSize,
    b + chunkSize,
    b - chunkSize * chunkSize,
    b + chunkSize * chunkSize,
  );
};

let get_exposed_face_verts = (b, chunk: array(Block.t)) => {
  let (behind, ahead, right, left, below, above) = get_block_neighbor_ids(b);
  let (behind, ahead, right, left, below, above) = (
    Utils.array_get_opt(behind, chunk),
    Utils.array_get_opt(ahead, chunk),
    Utils.array_get_opt(right, chunk),
    Utils.array_get_opt(left, chunk),
    Utils.array_get_opt(below, chunk),
    Utils.array_get_opt(above, chunk),
  );
  let (x, y, z) = Utils.index_to_3d_coord(b);
  let back_face =
    if (behind == Some(Air)) {
      Some((
        Vector3.create(x, y, z +. 1.),
        Vector3.create(x +. 1., y, z +. 1.),
        Vector3.create(x +. 1., y +. 1., z +. 1.),
        Vector3.create(x, y +. 1., z +. 1.),
      ));
    } else {
      None;
    };
  let front_face =
    if (ahead == Some(Air)) {
      Some((
        Vector3.create(x, y, z +. 1.),
        Vector3.create(x +. 1., y, z +. 1.),
        Vector3.create(x +. 1., y +. 1., z +. 1.),
        Vector3.create(x, y +. 1., z +. 1.),
      ));
    } else {
      None;
    };
  let right_face =
    if (right == Some(Air)) {
      Some((
        Vector3.create(x +. 1., y, z),
        Vector3.create(x +. 1., y, z +. 1.),
        Vector3.create(x +. 1., y +. 1., z +. 1.),
        Vector3.create(x +. 1., y +. 1., z),
      ));
    } else {
      None;
    };
  let left_face =
    if (left == Some(Air)) {
      Some((
        Vector3.create(x, y, z),
        Vector3.create(x, y, z +. 1.),
        Vector3.create(x, y +. 1., z +. 1.),
        Vector3.create(x, y +. 1., z),
      ));
    } else {
      None;
    };
  let bottom_face =
    if (below == Some(Air)) {
      Some((
        Vector3.create(x, y, z),
        Vector3.create(x, y, z +. 1.),
        Vector3.create(x +. 1., y, z +. 1.),
        Vector3.create(x +. 1., y, z),
      ));
    } else {
      None;
    };
  let top_face =
    if (above == Some(Air)) {
      Some((
        Vector3.create(x, y +. 1., z),
        Vector3.create(x, y +. 1., z +. 1.),
        Vector3.create(x +. 1., y +. 1., z +. 1.),
        Vector3.create(x +. 1., y +. 1., z),
      ));
    } else {
      None;
    };
  switch (back_face, front_face, right_face, left_face, bottom_face, top_face) {
  | (None, None, None, None, None, None) => None
  | _ =>
    Some(
      List.filter_map(
        x => x,
        [back_face, front_face, right_face, left_face, bottom_face, top_face],
      ),
    )
  };
};

let gen_chunk_mesh = (_ci: int, chunk: array(Block.t)) => {
  // verts is a bit dense, list(list(tuple(Vector3)))
  // It's a filtered list of blocks,
  // each block is a filtered list of faces,
  // each face is a 4-tuple of Vector3 coords

  let verts =
    List.mapi(
      (b, block: Block.t) =>
        if (block == Block.Air) {
          None;
        } else {
          get_exposed_face_verts(b, chunk);
        },
      Array.to_list(chunk),
    )
    |> List.filter_map(x => x);

  // This is not correct
  let flat_verts_array =
    List.flatten(verts)
    // For each face
    |> List.map(((v1, v2, v3, v4)) =>
         [
           [
             Vector3.x(v1),
             Vector3.y(v1),
             Vector3.z(v1),
             Vector3.x(v2),
             Vector3.y(v2),
             Vector3.z(v2),
             Vector3.x(v3),
             Vector3.y(v3),
             Vector3.z(v3),
           ],
           [
             Vector3.x(v1),
             Vector3.y(v1),
             Vector3.z(v1),
             Vector3.x(v4),
             Vector3.y(v4),
             Vector3.z(v4),
             Vector3.x(v3),
             Vector3.y(v3),
             Vector3.z(v3),
           ],
         ]
         |> List.flatten
       )
    |> List.flatten;

  let vertices_carray = Ctypes.CArray.of_list(Ctypes.float, flat_verts_array);

  // I think I need normals as well

  let chunk_mesh = Mesh.create();
  Mesh.set_vertex_count(chunk_mesh, List.length(flat_verts_array) / 3);
  Mesh.set_vertices(chunk_mesh, vertices_carray);

  chunk_mesh;
};

module MeshCache =
  Hashtbl.Make({
    type t = int;
    let equal = Int.equal;
    let hash = Hashtbl.hash;
  });

// module MeshCache =
//   Map.Make({
//     type t = int;
//     let compare = compare;
//   });

let mesh_cache: MeshCache.t(Mesh.t) = MeshCache.create(0);

// let get_mesh_for_chunk_opt = ci =>
//   switch (MeshCache.find(mesh_cache, ci)) {
//   | exception _exn => None
//   | chunk_mesh => Some(chunk_mesh)
//   };

// let generate_mesh_side_effect = ci =>
//   switch (World.get_chunk_at_index_opt(ci)) {
//   | None => ()
//   | Some(chunk) =>
//     switch (MeshCache.find(mesh_cache, ci)) {
//     | exception _exn =>
//       let chunk_mesh = gen_chunk_mesh(ci, chunk);
//       MeshCache.add(mesh_cache, ci, chunk_mesh);
//     | _chunk_mesh =>
//       let chunk_mesh = gen_chunk_mesh(ci, chunk);
//       MeshCache.replace(mesh_cache, ci, chunk_mesh);
//     }
//   };

let get_mesh_for_chunk_at_index = ci => {
  switch (MeshCache.find(mesh_cache, ci)) {
  | exception _exn =>
    // Generates the chunk if missing from the cache
    let chunk = World.get_chunk_at_index(ci);
    let chunk_mesh = gen_chunk_mesh(ci, chunk);
    upload_mesh(addr(chunk_mesh), false);
    MeshCache.add(mesh_cache, ci, chunk_mesh);
    chunk_mesh;
  | chunk_mesh => chunk_mesh
  };
};
