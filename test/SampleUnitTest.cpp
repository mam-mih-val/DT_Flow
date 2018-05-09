//
// Created by Lukas Kreis on 18.04.18.
//

#include <gtest/gtest.h>
#include <Base/Sample.h>
#include <TRandom3.h>

TEST(SampleUnitTest, create) {
  TRandom3 rndm;
  int nsamples = 1000;
  Qn::Sample a(nsamples);

  for (int i = 0; i < 10000000; ++i) {
    auto g = rndm.Gaus(0., 1.);
    a.Fill(g, {(unsigned int) (i / 10000.)});
  }
  a.CalculateCorrelatedError();
  EXPECT_NEAR(a.Error(),a.CorrelatedError(),a.Error()*0.01);
}

TEST(SampleUnitTest, add) {
  TRandom3 rndm;
  int nsamples = 1000;
  Qn::Sample a(nsamples);
  Qn::Sample b(nsamples);
  Qn::Sample d(nsamples);
  for (int i = 0; i < 10000000; ++i) {
    auto g1 = rndm.Gaus(0., 1.);
    auto g2 = rndm.Gaus(0., 1.);
    a.Fill(g1, {static_cast<unsigned int>(i / 10000.)});
    b.Fill(g2, {static_cast<unsigned int>(i / 10000.)});
    d.Fill(g1, {static_cast<unsigned int>(i / 10000.)});
    d.Fill(g2, {static_cast<unsigned int>(i / 10000.)});
  }
  auto c = a + b;
  d.CalculateCorrelatedError();
  EXPECT_FLOAT_EQ(c.Entries(),a.Entries()+b.Entries());
  EXPECT_FLOAT_EQ(c.Entries(),d.Entries());
  EXPECT_FLOAT_EQ(c.Mean(),d.Mean());
  EXPECT_FLOAT_EQ(c.Error(),d.Error());
  EXPECT_FLOAT_EQ(c.Mean(),(a.Mean()*a.Entries() + b.Mean()*b.Entries())/(a.Entries()+b.Entries()));

  EXPECT_NEAR(c.Error(),c.CorrelatedError(),c.Error()*0.05);
  EXPECT_FLOAT_EQ(c.CorrelatedError(),d.CorrelatedError());

  auto subc = d - a;
  subc.CalculateCorrelatedError();
  EXPECT_NEAR(subc.CorrelatedError(),b.Error(),b.Error()*0.01);

}