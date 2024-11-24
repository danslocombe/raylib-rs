#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(clippy::approx_constant)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

#[cfg(target_os = "macos")]
pub const MAX_MATERIAL_MAPS: u32 = 12;

impl Default for TraceLogLevel {
    fn default() -> Self {
        TraceLogLevel::LOG_INFO
    }
}

impl Default for Vector2 {
    fn default() -> Self {
        Self {
            x: 0.0,
            y: 0.0,
        }
    }
}

impl Vector2 {
    pub const fn zero() -> Self {
        Self {
            x: 0.0,
            y: 0.0,
        }
    }

    pub fn new(x: f32, y: f32) -> Self {
        Self {
            x,
            y,
        }
    }

    pub fn to_rect(self, width: f32, height: f32) -> Rectangle {
        Rectangle {
            x: self.x,
            y: self.y,
            width,
            height,
        }
    }
}

impl std::ops::Add for Vector2 {
    type Output = Vector2;

    fn add(self, rhs: Self) -> Self::Output {
        Vector2 {
            x: self.x + rhs.x,
            y: self.y + rhs.y,
        }
    }
}

impl std::ops::AddAssign for Vector2 {
    fn add_assign(&mut self, rhs: Self) {
        self.x = self.x + rhs.x;
        self.y = self.y + rhs.y;
    }
}

impl std::ops::Sub for Vector2 {
    type Output = Vector2;

    fn sub(self, rhs: Self) -> Self::Output {
        Vector2 {
            x: self.x - rhs.x,
            y: self.y - rhs.y,
        }
    }
}

impl std::ops::SubAssign for Vector2 {
    fn sub_assign(&mut self, rhs: Self) {
        self.x = self.x - rhs.x;
        self.y = self.y - rhs.y;
    }
}

impl std::ops::Mul<Vector2> for f32 {
    type Output = Vector2;
    
    fn mul(self, rhs: Vector2) -> Self::Output {
        Vector2::new(self * rhs.x,self * rhs.y)
    }

}

impl std::ops::MulAssign<f32> for Vector2 {
    fn mul_assign(&mut self, rhs: f32) {
        self.x = self.x * rhs;
        self.y = self.y * rhs;
    }
}

impl std::ops::Mul<f32> for Vector2 {
    type Output = Vector2;
    
    fn mul(self, rhs: f32) -> Self::Output {
        Vector2::new(self.x * rhs,self.y * rhs)
    }

}

impl Default for Rectangle {
    fn default() -> Self {
        Self {
            x: 0.0,
            y: 0.0,
            width: 0.0,
            height: 0.0,
        }
    }
}


impl Rectangle {
    pub const fn zero() -> Self {
        Self {
            x: 0.0,
            y: 0.0,
            width: 0.0,
            height: 0.0,
        }
    }

    pub fn top_left(&self) -> Vector2 {
        Vector2::new(self.x, self.y)
    }

    pub fn contains(&self, p: Vector2) -> bool {
        p.x > self.x && p.x < self.x + self.width
        &&
        p.y > self.y && p.y < self.y + self.height
    }
}

impl std::ops::Add<Vector2> for Rectangle {
    type Output = Rectangle;

    fn add(self, rhs: Vector2) -> Self::Output {
        Self {
            x: self.x + rhs.x,
            y: self.y + rhs.y,
            width: self.width,
            height: self.height,
        }
    }
}

impl std::ops::Sub<Vector2> for Rectangle {
    type Output = Rectangle;

    fn sub(self, rhs: Vector2) -> Self::Output {
        Self {
            x: self.x - rhs.x,
            y: self.y - rhs.y,
            width: self.width,
            height: self.height,
        }
    }
}