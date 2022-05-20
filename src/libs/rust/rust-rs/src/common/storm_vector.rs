use glam::{DVec3, Vec3A, Vec4};

pub struct VecWrapper3F {
    vec: Vec3A,
}

pub struct VecWrapper3D {
    vec: DVec3,
}

pub struct VecWrapper4F {
    vec: Vec4,
}

impl VecWrapper3F {
    fn to_boxed(self) -> *mut Self {
        let boxed = Box::new(self);
        Box::into_raw(boxed)
    }
}

impl VecWrapper3D {
    fn to_boxed(self) -> *mut Self {
        let boxed = Box::new(self);
        Box::into_raw(boxed)
    }
}

impl VecWrapper4F {
    fn to_boxed(self) -> *mut Self {
        let boxed = Box::new(self);
        Box::into_raw(boxed)
    }
}

mod export {
    use glam::{DVec3, Vec3A, Vec4};

    use super::{VecWrapper3D, VecWrapper3F, VecWrapper4F};

    #[no_mangle]
    pub unsafe extern "C" fn ffi_new_3f_vector(x: f32, y: f32, z: f32) -> *mut VecWrapper3F {
        let vec = VecWrapper3F {
            vec: Vec3A::new(x, y, z),
        };
        vec.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_free_3f_vector(ptr: *mut VecWrapper3F) {
        if ptr.is_null() {
            return;
        }
        let _ = Box::from_raw(ptr);
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_new_3d_vector(x: f64, y: f64, z: f64) -> *mut VecWrapper3D {
        let vec = VecWrapper3D {
            vec: DVec3::new(x, y, z),
        };
        vec.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_free_3d_vector(ptr: *mut VecWrapper3D) {
        if ptr.is_null() {
            return;
        }
        let _ = Box::from_raw(ptr);
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_new_4f_vector(
        x: f32,
        y: f32,
        z: f32,
        w: f32,
    ) -> *mut VecWrapper4F {
        let vec = VecWrapper4F {
            vec: Vec4::new(x, y, z, w),
        };
        vec.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_free_4f_vector(ptr: *mut VecWrapper4F) {
        if ptr.is_null() {
            return;
        }
        let _ = Box::from_raw(ptr);
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_x(v: *mut VecWrapper3F) -> f32 {
        (*v).vec.x
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_y(v: *mut VecWrapper3F) -> f32 {
        (*v).vec.y
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_z(v: *mut VecWrapper3F) -> f32 {
        (*v).vec.z
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_x(v: *mut VecWrapper3D) -> f64 {
        (*v).vec.x
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_y(v: *mut VecWrapper3D) -> f64 {
        (*v).vec.y
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_z(v: *mut VecWrapper3D) -> f64 {
        (*v).vec.z
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector4f_x(v: *mut VecWrapper4F) -> f32 {
        (*v).vec.x
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector4f_y(v: *mut VecWrapper4F) -> f32 {
        (*v).vec.y
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector4f_z(v: *mut VecWrapper4F) -> f32 {
        (*v).vec.z
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector4f_w(v: *mut VecWrapper4F) -> f32 {
        (*v).vec.w
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_add_self(v1: *mut VecWrapper3F, v2: *mut VecWrapper3F) {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        v1.vec += v2.vec;
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_sub_self(v1: *mut VecWrapper3F, v2: *mut VecWrapper3F) {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        v1.vec -= v2.vec;
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_mul_self(v1: *mut VecWrapper3F, v2: *mut VecWrapper3F) {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        v1.vec *= v2.vec;
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_div_self(v1: *mut VecWrapper3F, v2: *mut VecWrapper3F) {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        v1.vec /= v2.vec;
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_add(
        v1: *mut VecWrapper3F,
        v2: *mut VecWrapper3F,
    ) -> *mut VecWrapper3F {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        let vec = v1.vec + v2.vec;
        VecWrapper3F { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_sub(
        v1: *mut VecWrapper3F,
        v2: *mut VecWrapper3F,
    ) -> *mut VecWrapper3F {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        let vec = v1.vec - v2.vec;
        VecWrapper3F { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_mul(
        v1: *mut VecWrapper3F,
        v2: *mut VecWrapper3F,
    ) -> *mut VecWrapper3F {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        let vec = v1.vec * v2.vec;
        VecWrapper3F { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_div(
        v1: *mut VecWrapper3F,
        v2: *mut VecWrapper3F,
    ) -> *mut VecWrapper3F {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        let vec = v1.vec / v2.vec;
        VecWrapper3F { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_mul_f32(
        v1: *mut VecWrapper3F,
        f: f32,
    ) -> *mut VecWrapper3F {
        let v1 = v1.as_mut().unwrap();
        let vec = v1.vec * f;
        VecWrapper3F { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_div_f32(
        v1: *mut VecWrapper3F,
        f: f32,
    ) -> *mut VecWrapper3F {
        let v1 = v1.as_mut().unwrap();
        let vec = v1.vec / f;
        VecWrapper3F { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_length_sq(v1: *mut VecWrapper3F) -> f32 {
        let v1 = v1.as_mut().unwrap();
        v1.vec.length_squared()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_length(v1: *mut VecWrapper3F) -> f32 {
        let v1 = v1.as_mut().unwrap();
        v1.vec.length()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_normal(v1: *mut VecWrapper3F) -> *mut VecWrapper3F {
        let v1 = v1.as_mut().unwrap();
        let vec = v1.vec.normalize_or_zero();
        VecWrapper3F { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_cross(
        v1: *mut VecWrapper3F,
        v2: *mut VecWrapper3F,
    ) -> *mut VecWrapper3F {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        let vec = v1.vec.cross(v2.vec);
        VecWrapper3F { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3f_dot(v1: *mut VecWrapper3F, v2: *mut VecWrapper3F) -> f32 {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        v1.vec.dot(v2.vec)
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_add_self(v1: *mut VecWrapper3D, v2: *mut VecWrapper3D) {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        v1.vec += v2.vec;
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_sub_self(v1: *mut VecWrapper3D, v2: *mut VecWrapper3D) {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        v1.vec -= v2.vec;
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_mul_self(v1: *mut VecWrapper3D, v2: *mut VecWrapper3D) {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        v1.vec *= v2.vec;
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_div_self(v1: *mut VecWrapper3D, v2: *mut VecWrapper3D) {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        v1.vec /= v2.vec;
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_add(
        v1: *mut VecWrapper3D,
        v2: *mut VecWrapper3D,
    ) -> *mut VecWrapper3D {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        let vec = v1.vec + v2.vec;
        VecWrapper3D { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_sub(
        v1: *mut VecWrapper3D,
        v2: *mut VecWrapper3D,
    ) -> *mut VecWrapper3D {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        let vec = v1.vec - v2.vec;
        VecWrapper3D { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_mul(
        v1: *mut VecWrapper3D,
        v2: *mut VecWrapper3D,
    ) -> *mut VecWrapper3D {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        let vec = v1.vec * v2.vec;
        VecWrapper3D { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_div(
        v1: *mut VecWrapper3D,
        v2: *mut VecWrapper3D,
    ) -> *mut VecWrapper3D {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        let vec = v1.vec / v2.vec;
        VecWrapper3D { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_mul_f64(
        v1: *mut VecWrapper3D,
        f: f64,
    ) -> *mut VecWrapper3D {
        let v1 = v1.as_mut().unwrap();
        let vec = v1.vec * f;
        VecWrapper3D { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_div_f64(
        v1: *mut VecWrapper3D,
        f: f64,
    ) -> *mut VecWrapper3D {
        let v1 = v1.as_mut().unwrap();
        let vec = v1.vec / f;
        VecWrapper3D { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_length_sq(v1: *mut VecWrapper3D) -> f64 {
        let v1 = v1.as_mut().unwrap();
        v1.vec.length_squared()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_length(v1: *mut VecWrapper3D) -> f64 {
        let v1 = v1.as_mut().unwrap();
        v1.vec.length()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_normal(v1: *mut VecWrapper3D) -> *mut VecWrapper3D {
        let v1 = v1.as_mut().unwrap();
        let vec = v1.vec.normalize_or_zero();
        VecWrapper3D { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_cross(
        v1: *mut VecWrapper3D,
        v2: *mut VecWrapper3D,
    ) -> *mut VecWrapper3D {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        let vec = v1.vec.cross(v2.vec);
        VecWrapper3D { vec }.to_boxed()
    }

    #[no_mangle]
    pub unsafe extern "C" fn ffi_vector3d_dot(v1: *mut VecWrapper3D, v2: *mut VecWrapper3D) -> f64 {
        let v1 = v1.as_mut().unwrap();
        let v2 = v2.as_mut().unwrap();
        v1.vec.dot(v2.vec)
    }
}
