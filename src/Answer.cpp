//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2018 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------
#include "Answer.hpp"

#include <limits>
#include <typeinfo>
#include <initializer_list>

#include <tuple>
#include <type_traits>
#include <memory>
#include <bitset>

#include <array>
#include <deque>
#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <iterator>

#include <algorithm>

#include <random>
#include <valarray>
#include <numeric>

#include <cmath>

using namespace std;


#define ALL(v) (v).begin(),(v).end()
// #define repeat(cnt,l) for(remove_reference<remove_const<decltype(l)>::type>::type cnt=0;(cnt)<(l);++(cnt))
#define repeat(cnt,l) for(auto cnt=0ll;(cnt)<(l);++(cnt))
#define rrepeat(cnt,l) for(auto cnt=(l)-1;0<=(cnt);--(cnt))
#define iterate(cnt,b,e) for(auto cnt=(b);(cnt)!=(e);++(cnt))
#define diterate(cnt,b,e) for(auto cnt=(b);(cnt)!=(e);--(cnt))

//------------------------------------------------------------------------------
namespace hpc {

//------------------------------------------------------------------------------
    namespace util {


        void solvePlacement(const Oven& oven, const CandidateLane& smallLane, const CandidateLane& largeLane) {
            
        }
    }
//------------------------------------------------------------------------------
/// コンストラクタ。
/// @detail 最初のステージ開始前に実行したい処理があればここに書きます。
Answer::Answer()
{
}

//------------------------------------------------------------------------------
/// デストラクタ。
/// @detail 最後のステージ終了後に実行したい処理があればここに書きます。
Answer::~Answer()
{
}

//------------------------------------------------------------------------------
/// 各ステージ開始時に呼び出される処理。
/// @detail 各ステージに対する初期化処理が必要ならここに書きます。
/// @param aStage 現在のステージ。
void Answer::init(const Stage& aStage)
{
    auto copyedStage = aStage;
}

//------------------------------------------------------------------------------
/// このターンでの行動を指定する。
/// @detail 希望する行動を Action の static 関数で生成して return してください。
///         正常ではない行動を return した場合、何も起きません。
///         詳細は Action のヘッダを参照してください。
/// @param aStage 現在のステージ。
Action Answer::decideNextAction(const Stage& aStage)
{
    auto& laneS = aStage.candidateLane(CandidateLaneType_Small);
    auto& laneL = aStage.candidateLane(CandidateLaneType_Large);
    auto& oven = aStage.oven();


    repeat(i, laneL.pieces().count()) {
        repeat(y, oven.height()) {
            rrepeat(x, oven.width()) {
                if (aStage.oven().isAbleToPut(laneL.pieces()[i], Vector2i(x, y))) {
                    return Action::Put(CandidateLaneType_Large, i, Vector2i(x, y));
                }
            }
        }
    }
    repeat(i, laneL.pieces().count()) {
        rrepeat(y, oven.height()) {
            repeat(x, min(oven.width(), 5)) {
                if (aStage.oven().isAbleToPut(laneS.pieces()[i], Vector2i(x, y))) {
                    return Action::Put(CandidateLaneType_Small, i, Vector2i(x, y));
                }
            }
        }
    }

    // 配置できないなら、このターンは何もしない
    return Action::Wait();
}

//------------------------------------------------------------------------------
/// 各ステージ終了時に呼び出される処理。
/// @detail 各ステージに対する終了処理が必要ならここに書きます。
/// @param aStage 現在のステージ。
void Answer::finalize(const Stage& aStage)
{
}

} // namespace
// EOF
