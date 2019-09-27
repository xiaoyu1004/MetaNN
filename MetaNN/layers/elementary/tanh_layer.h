#pragma once
#include <MetaNN/layers/facilities/common_io.h>
#include <MetaNN/layers/facilities/policies.h>
#include <MetaNN/layers/facilities/traits.h>
#include <MetaNN/policies/policy_operations.h>
#include <MetaNN/policies/policy_selector.h>
#include <stack>

namespace MetaNN
{
    namespace NSTanhLayer
    {
        template <bool IsFeedbackOutput, typename TInputType>
        struct InternalDataTypeCalculator
        {
            using type = NullParameter;
        };
        
        template <typename TInputType>
        struct InternalDataTypeCalculator<true, TInputType>
        {
            using OutputType = decltype(Tanh(std::declval<TInputType>()));
            using type = LayerTraits::LayerInternalBuf<OutputType, true>;
        };
    }

    
    template <typename TInputs, typename TPolicies>
    class TanhLayer
    {
        static_assert(IsPolicyContainer<TPolicies>);
        using CurLayerPolicy = PlainPolicy<TPolicies>;

    public:
        static constexpr bool IsFeedbackOutput = PolicySelect<GradPolicy, CurLayerPolicy>::IsFeedbackOutput;
        static constexpr bool IsUpdate = false;

        using InputPortSet = LayerInputPortSet<TanhLayer>;
        using OutputPortSet = LayerOutputPortSet<TanhLayer>;
        using InputMap = TInputs;
        
    private:
        using TLayerInputFP = typename InputMap::template Find<LayerInput>;

    public:
        TanhLayer(std::string name)
            : m_name(std::move(name))
        {}
        
        template <typename TIn>
        auto FeedForward(TIn&& p_in)
        {
            auto val = LayerTraits::PickItemFromCont<InputMap, LayerInput>(std::forward<TIn>(p_in));
            auto res = Tanh(val);

            if constexpr (IsFeedbackOutput)
            {
                m_inputShape.PushDataShape(val);
                m_data.push(res);
            }
            return LayerOutputCont<TanhLayer>().template Set<LayerOutput>(std::move(res));
        }

        template <typename TGrad>
        auto FeedBackward(TGrad&& p_grad)
        {
            if constexpr (IsFeedbackOutput)
            {
                if (m_data.empty())
                {
                    throw std::runtime_error("Cannot feed back in TanhLayer");
                }
                auto tanhRes = m_data.top();
                m_data.pop();

                auto grad = std::forward<TGrad>(p_grad).template Get<LayerOutput>();

                auto res = TanhGrad(std::move(grad), std::move(tanhRes));
                m_inputShape.CheckDataShapeAndPop(res);
                return LayerInputCont<TanhLayer>().template Set<LayerInput>(std::move(res));
            }
            else
            {
                return LayerInputCont<TanhLayer>();
            }
        }

        void NeutralInvariant() const
        {
            if constexpr(IsFeedbackOutput)
            {
                if (!m_data.empty())
                {
                    throw std::runtime_error("NeutralInvariant Fail!");
                }
                m_inputShape.AssertEmpty();
            }
        }
    private:
        std::string m_name;
        using InternalDataType = typename NSTanhLayer::InternalDataTypeCalculator<IsFeedbackOutput, TLayerInputFP>::type;
        InternalDataType m_data;

        LayerTraits::ShapeChecker<TLayerInputFP,  IsFeedbackOutput> m_inputShape;
    };
}