// Flow Vector Correction Framework
//
// Copyright (C) 2018  Lukas Kreis, Ilya Selyuzhenkov
// Contact: l.kreis@gsi.de; ilya.selyuzhenkov@gmail.com
// For a full list of contributors please see docs/Credits
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "Profile.h"

namespace Qn {

Profile Profile::MergeNormal(const Profile &lhs, const Profile &rhs) {
  Profile result;
  result.sumwy_ = lhs.sumwy_ + rhs.sumwy_;
  result.sumwy2_ = lhs.sumwy2_ + rhs.sumwy2_;
  result.sumw_ = lhs.sumw_ + rhs.sumw_;
  result.sumw2_ = lhs.sumw2_ + rhs.sumw2_;
  result.entries_ = lhs.entries_ + rhs.entries_;
  return result;
}
Profile Profile::MergePointAverage(const Profile &lhs, const Profile &rhs) {
  Profile result;
  (void)lhs;
  (void)rhs;
  std::cout << "not mergeable" << std::endl;
  return result;
}
Profile Profile::AdditionNormal(const Profile &lhs, const Profile &rhs) {
  Profile result;
  result.sumwy_ = lhs.sumwy_ + rhs.sumwy_;
  result.sumwy2_ = lhs.sumwy2_ + rhs.sumwy2_;
  result.sumw_ = lhs.sumw_ + rhs.sumw_;
  result.sumw2_ = lhs.sumw2_ + rhs.sumw2_;
  result.entries_ = lhs.entries_ + rhs.entries_;
  return result;
}
Profile Profile::AdditionPointAverage(const Profile &lhs, const Profile &rhs) {
  Profile result;
  result.mean_ = lhs.mean_ + rhs.mean_;
  result.var_ = sqrt(lhs.var_ * lhs.var_ + rhs.var_ * rhs.var_);
  return result;
}
Profile Profile::SubtractionNormal(const Profile &lhs, const Profile &rhs) {
  Profile result;
  result.sumwy_ = lhs.sumwy_ - rhs.sumwy_;
  result.sumwy2_ = lhs.sumwy2_ - rhs.sumwy2_;
  result.sumw_ = lhs.sumw_ - rhs.sumw_;
  result.sumw2_ = lhs.sumw2_ - rhs.sumw2_;
  result.entries_ = lhs.entries_ - rhs.entries_;
  return result;
}

Profile Profile::SubtractionPointAverage(const Profile &lhs,
                                         const Profile &rhs) {
  Profile result;
  result.mean_ = lhs.mean_ - rhs.mean_;
  result.var_ = sqrt(lhs.var_ * lhs.var_ + rhs.var_ * rhs.var_);
  return result;
}

Profile Profile::MultiplicationNormal(const Profile &lhs, const Profile &rhs) {
  Profile result;
  result.sumwy_ = lhs.sumwy_ * rhs.sumwy_;
  result.sumwy2_ = lhs.sumwy2_ * rhs.sumwy2_;
  result.sumw_ = lhs.sumw_ * rhs.sumw_;
  result.sumw2_ = lhs.sumw2_ * rhs.sumw2_;
  result.entries_ = lhs.entries_ + rhs.entries_;
  return result;
}

Profile Profile::MultiplicationPointAverage(const Profile &lhs,
                                            const Profile &rhs) {
  Profile result;
  result.mean_ = lhs.mean_ * rhs.mean_;
  result.var_ = sqrt(rhs.mean_ * rhs.mean_ * lhs.var_ * lhs.var_ +
                     lhs.mean_ * lhs.mean_ * rhs.var_ * rhs.var_);
  return result;
}

Profile Profile::DivisionNormal(const Profile &num, const Profile &den) {
  Profile result;
  result.sumwy_ = num.sumwy_ / den.sumwy_;
  result.sumwy2_ = num.sumwy2_ / den.sumwy2_;
  result.sumw_ = num.sumw_ / den.sumw_;
  result.sumw2_ = num.sumw2_ / den.sumw2_;
  result.entries_ = num.entries_;
  return result;
}

Profile Profile::DivisionPointAverage(const Profile &num, const Profile &den) {
  Profile result;
  result.mean_ = num.mean_ / den.mean_;
  double den2 = den.mean_ * den.mean_;
  result.var_ = sqrt((den2 * num.var_ * num.var_ +
                      num.mean_ * num.mean_ * den.var_ * den.var_) /
                     (den2 * den2));
  return result;
}

Profile Profile::SqrtNormal(const Profile &prof) {
  Profile result;
  result.sumwy_ = std::signbit(prof.sumwy_) ? -1 * sqrt(fabs(prof.sumwy_))
                                            : sqrt(fabs(prof.sumwy_));
  result.sumwy2_ = sqrt(prof.sumwy2_);
  result.sumw_ = sqrt(prof.sumw_);
  result.sumw2_ = sqrt(prof.sumw2_);
  result.entries_ = prof.entries_;
  return result;
}

Profile Profile::SqrtPointAverage(const Profile &prof) {
  Profile result;
  result.mean_ = sqrt(prof.mean_);
  result.var_ = sqrt(prof.var_);
  return result;
}

Profile Profile::ResFullEventNormal(const Profile &prof) {
  Profile result;
  double mean = prof.sumwy_ / prof.sumw_;
  double variance = fabs(prof.sumwy2_ / prof.sumw_ - mean * mean);
  double neff = prof.sumw_ * prof.sumw_ / prof.sumw2_;
  double var;
  if (neff > 0.) {
    var = std::sqrt(variance / neff);
  } else {
    var = 0.;
  }

  double res = GetFullResolution(mean);
  double delta = var / mean * res;
  result.sumwy_ = res * prof.entries_;
  result.sumw_ = prof.entries_;
  result.sumwy2_ = sqrt(res + delta);
  result.sumw2_ = prof.entries_;
  result.entries_ = prof.entries_;
  return result;
}

Profile Profile::ResFullEventElliptic(const Profile &prof) {
  Profile result;
  double mean = prof.sumwy_ / prof.sumw_;
  double variance = fabs(prof.sumwy2_ / prof.sumw_ - mean * mean);
  double neff = prof.sumw_ * prof.sumw_ / prof.sumw2_;
  double var;
  if (neff > 0.) {
    var = std::sqrt(variance / neff);
  } else {
    var = 0.;
  }
  if (neff > 0.) {
    var = std::sqrt(variance / neff);
  } else {
    var = 0.;
  }
  auto fact = [](int k) {
    double result = 1;
    for (int i = 1; i <= k; ++i)
      result *= i;
    return result;
  };
  auto I = [fact](double nu, double z) {
    double result = 0;
    for (int k = 0; k < 10; ++k)
      result +=
          pow(z / 2.0, 2 * k + nu) / (fact(k) * TMath::Gamma(k + nu + 1));
    return result;
  };
  auto R = [I](double chi, double k) {
    double chi2_2 = chi * chi / 2;
    double result = sqrt(TMath::Pi()) / 2.0 * chi * exp(-chi2_2) *
                   (I((k-1.0)/2, chi2_2) + I((k+1.0)/2, chi2_2));
    return result;
  };
  auto f = [R,mean](double chi) { return R(chi, 1.0)-sqrt(2*mean); };
  double a = 0.0;
  double b = 3.0;
  int i = 0;
  while (fabs(a - b) > pow(10, -6)) {
    double c = (a + b) / 2;
    double fc = f(c);
    if (fc == 0)
      break;
    double fa = f(a);
    double fb = f(b);
    if (fa * fc < 0.0) {
      b = c;
      i++;
      continue;
    }
    if (fb * fc < 0.0) {
      a = c;
      i++;
      continue;
    }
  }
  double chi = (a + b) / 2;
  double res = R(sqrt(2) * chi, 2);
  double delta = var / mean * res;
  result.sumwy_ = res * prof.entries_;
  result.sumw_ = prof.entries_;
  result.sumwy2_ = sqrt(res + delta);
  result.sumw2_ = prof.entries_;
  result.entries_ = prof.entries_;
  return result;
}

double Profile::GetFullResolution(double XX) {
  std::string formulae = "sqrt(pi)/2*x*exp(-x^2/2)*(TMath::BesselI0(x^2/"
                         "2)+TMath::BesselI1(x^2/2))";
  std::string equation = formulae + "-" + std::to_string(sqrt(2 * XX));
  TF1 f("equation", equation.data(), 0.0, 3.0);
  // f.Draw();
  double a = 0.0;
  double b = 3.0;
  int i = 0;
  while (fabs(a - b) > pow(10, -6)) {
    double c = (a + b) / 2;
    double fc = f(c);
    if (fc == 0)
      break;
    double fa = f(a);
    double fb = f(b);
    if (fa * fc < 0.0) {
      b = c;
      i++;
      continue;
    }
    if (fb * fc < 0.0) {
      a = c;
      i++;
      continue;
    }
  }
  double chi = (a + b) / 2;
  f = TF1("resolution", formulae.data(), 0.0, 3.0);
  return f(sqrt(2) * chi);
}

Profile Profile::ScaleNormal(const Profile &lhs, double scale) {
  Profile result;
  result.sumwy_ = lhs.sumwy_ * scale;
  result.sumwy2_ = lhs.sumwy2_ * scale;
  result.sumw_ = lhs.sumw_;
  result.sumw2_ = lhs.sumw2_;
  result.entries_ = lhs.entries_;
  return result;
}

Profile Profile::ScalePointAverage(const Profile &lhs, double scale) {
  Profile result;
  result.mean_ = scale * lhs.mean_;
  result.var_ = scale * lhs.var_;
  return result;
}

} // namespace Qn