#include "HGCSSCluster.hh"

#include <iomanip>
#include <cmath>
#include <stdlib.h>

HGCSSCluster::HGCSSCluster(const HGCSSRecoHit & aRecHit){
  energy_ = aRecHit.energy();
  //mm->cm
  pos_ = aRecHit.position();

  layer_ = aRecHit.layer();

  vtx_=ROOT::Math::XYZPoint(0,0,0);

}

void HGCSSCluster::addRecHitFraction(std::pair<HGCSSRecoHit*,double> aHit){
  std::pair<std::map<HGCSSRecoHit*,double>::iterator,bool> isInserted = recHitMap_.insert(aHit);
  if (!isInserted.second) isInserted.first->second += aHit.second;
}

std::map<HGCSSRecoHit*,double> HGCSSCluster::recHitFractions() const{
  return recHitMap_;
}


void HGCSSCluster::calculatePosition(){
  std::map<HGCSSRecoHit*,double>::iterator iter = recHitMap_.begin();
  double xpos = 0;
  double ypos = 0;
  double zpos = 0;
  double etot = 0;
  unsigned minlayer = 1000;
  unsigned maxlayer = 0;
  for (;iter!=recHitMap_.end();++iter){
    //const HGCSSRecoHit* lhit = iter->first;
    double en = (iter->first)->energy();
    unsigned layer = (iter->first)->layer();
    etot += en;
    ROOT::Math::XYZPoint pos((iter->first)->position());
    xpos += en*pos.x();
    ypos += en*pos.y();
    zpos += en*pos.z();
    if (layer>maxlayer) maxlayer=layer;
    if (layer<minlayer) minlayer=layer;
  }
  if (etot>0)
    pos_ = ROOT::Math::XYZPoint(xpos/etot,ypos/etot,zpos/etot);
  else pos_ = seedPos_;
  energy_ = etot;
  width_ = maxlayer-minlayer;
}

double HGCSSCluster::theta() const {

  return 2*atan(exp(-1.*eta()));
}

double HGCSSCluster::eta() const {
  double tanx = (pos_.x()-vtx_.x())/(pos_.z()-vtx_.z());
  double tany = (pos_.y()-vtx_.y())/(pos_.z()-vtx_.z());
  return asinh(1.0/sqrt(tanx*tanx+tany*tany));
}

double HGCSSCluster::getSeedEta() const {
  double tanx = (seedPos_.x()-vtx_.x())/(seedPos_.z()-vtx_.z());
  double tany = (seedPos_.y()-vtx_.y())/(seedPos_.z()-vtx_.z());
  return asinh(1.0/sqrt(tanx*tanx+tany*tany));
}

double HGCSSCluster::phi() const {
  double tanx = (pos_.x()-vtx_.x())/(pos_.z()-vtx_.z());
  double tany = (pos_.y()-vtx_.y())/(pos_.z()-vtx_.z());
  return atan2(tany,tanx);
}

double HGCSSCluster::getSeedPhi() const {
  double tanx = (seedPos_.x()-vtx_.x())/(seedPos_.z()-vtx_.z());
  double tany = (seedPos_.y()-vtx_.y())/(seedPos_.z()-vtx_.z());
  return atan2(tany,tanx);
}

void HGCSSCluster::Print(std::ostream & aOs) const{
  aOs << std::endl
      << "=== Layer " << layer_ << "\t width " << width_
      << "\t Nhits " << recHitMap_.size()
      << "\t E " << energy_ << "\t seedE " << seedE_ 
      << "\t ==="<< std::endl
      << "=== eta,phi " << eta() << " " << phi() 
      << "\t seed eta,phi " << getSeedEta() << " " << getSeedPhi()
      << "\t ===" << std::endl;

}
