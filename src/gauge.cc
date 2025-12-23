#include"zgui.h"
#include <numbers>
#include <cmath>
#include <complex>
#include <iostream>
using namespace std;
using namespace cv;

z::Gauge::Gauge(int start, int end, int step, int nth, cv::Rect2i r) : z::Widget(r)
{
  radius_ = double{r.width} / 2 - margin_;
  center_.x = margin_ + radius_;
  center_.y = margin_ + radius_;
  start_ = start; end_ = end; step_ = step; nth_ = nth;
  start_angle_ = numbers::pi * -3 / 4;
  end_angle_ = numbers::pi * -1 / 4;
  draw_scale();
  draw_needle(0);
  update();
}

double z::Gauge::total_angle() const
{
  double ang = start_angle_ - end_angle_;
  if(ang < 0) ang = -ang;
  while(ang > numbers::pi * 2) ang -= numbers::pi * 2;
  if(ang < numbers::pi) ang = numbers::pi * 2 - ang;
  return ang;
}


void z::Gauge::draw_scale()
{
  mat_ = back_;
  double step_angle = total_angle() / -abs(end_ - start_);
  complex<double> z1, z2, z3;
  cv::Size size;
  double angle, font_height;
  string text;
  for(int i=start_,j=0, baseline=0; i<=end_; i+=step_, j++)
  { 
    angle = start_angle_ + step_angle * i;
    z1 = polar(radius_, angle);
    z2 = polar(radius_ - ((j % nth_ == 0) ? 2 : 1) * radius_ / 8 , angle);
    cv::line
    ( mat_
    , {z1.real() + center_.x, -z1.imag() + center_.y}
    , {z2.real() + center_.x, -z2.imag() + center_.y}
    , {0, 255, 0}
    , 3
    );
    if(j % nth_ == 0) 
    { z3 = polar(radius_ * 10 / 16, angle);
      text = to_string(start_ + i);
      font_height = radius_ * 0.15;
      size = ft2_->getTextSize(text, font_height, 4, &baseline);
      ft2_->putText
      ( mat_
      , text
      , { z3.real() + center_.x - size.width / 2
        , -z3.imag() + center_.y - size.height / 2 - baseline
        }
      , font_height
      , {0, 255, 0}
      , -1
      , 4
      , false
      );
    }
  }
}

void z::Gauge::draw_needle(float value)
{
  cv::circle(mat_, center_, 10, {0,0,255}, -1); 
  float angle = start_angle_ - float{value - start_} / (end_ - start_) * total_angle(); 
  auto z = polar(float{radius_} * 4 / 5, angle);
  cv::line(mat_, {z.real() + center_.x, -z.imag() + center_.y}, center_, {0, 0, 255}, 3);
}

void z::Gauge::value(float v)
{
  value_ = v;
  draw_scale();
  draw_needle(v);
  update();
}
