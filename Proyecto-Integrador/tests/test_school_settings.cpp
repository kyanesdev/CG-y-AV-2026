#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <dlfdm/models/aircraft/jettrainer.h>
#include "../src/modules/SchoolFlightSettings.h"

int main() {
    const auto base=dlfdm::jettrainer::load_model();
    auto tuned=base;
    aeroiua::configure_training_response(tuned);
    const float gain=aeroiua::kSpeedResponse;
    const auto approximately=[](float a,float b) {
        return std::fabs(a-b)<1e-4f*std::max(1.f,std::fabs(b));
    };
    assert(approximately(tuned.engine.max_thrust_sl,base.engine.max_thrust_sl*gain));
    assert(approximately(tuned.CD0,base.CD0*gain));
    assert(approximately(tuned.CDa,base.CDa*gain));
    assert(approximately(tuned.mass,base.mass));
    assert(approximately(tuned.CLa,base.CLa));

    // A la misma condicion de vuelo, tanto D como T escalan por igual.
    // Por eso el throttle de trim (~50%) NO debe recalibrarse en v0.6.
    const float alpha=std::atan2(1.521f,149.992f);
    const float drag_base=base.CD0+base.CDa*alpha;
    const float drag_tuned=tuned.CD0+tuned.CDa*alpha;
    assert(approximately(drag_tuned,drag_base*gain));
    const auto trim=dlfdm::jettrainer::get_trim_condition(
        dlfdm::jettrainer::TrimCondition::kISA5000TAS150);
    assert(trim.controls.throttle>0.49f && trim.controls.throttle<0.50f);
    std::cout<<"OK: empuje/arrastre x"<<gain<<", trim sin alterar, otros parametros conservados\n";
}
