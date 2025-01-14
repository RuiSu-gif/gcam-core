/*
* LEGAL NOTICE
* This computer software was prepared by Battelle Memorial Institute,
* hereinafter the Contractor, under Contract No. DE-AC05-76RL0 1830
* with the Department of Energy (DOE). NEITHER THE GOVERNMENT NOR THE
* CONTRACTOR MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
* LIABILITY FOR THE USE OF THIS SOFTWARE. This notice including this
* sentence must appear on any copies of this computer software.
* 
* EXPORT CONTROL
* User agrees that the Software will not be shipped, transferred or
* exported into any country or used in any manner prohibited by the
* United States Export Administration Act or any other applicable
* export laws, restrictions or regulations (collectively the "Export Laws").
* Export of the Software may require some form of license or other
* authority from the U.S. Government, and failure to obtain such
* export control license may result in criminal liability under
* U.S. laws. In addition, if the Software is identified as export controlled
* items under the Export Laws, User represents and warrants that User
* is not a citizen, or otherwise located within, an embargoed nation
* (including without limitation Iran, Syria, Sudan, Cuba, and North Korea)
*     and that User is not otherwise prohibited
* under the Export Laws from receiving the Software.
* 
* Copyright 2011 Battelle Memorial Institute.  All Rights Reserved.
* Distributed as open-source under the terms of the Educational Community 
* License version 2.0 (ECL 2.0). http://www.opensource.org/licenses/ecl2.php
* 
* For further details, see: http://www.globalchange.umd.edu/models/gcam/
*
*/


#ifndef _ENERGY_FINAL_DEMAND_H_
#define _ENERGY_FINAL_DEMAND_H_
#if defined(_MSC_VER)
#pragma once
#endif

/*! 
 * \file energy_final_demand.h
 * \ingroup Objects
 * \brief The EnergyFinalDemand class header file.
 * \author Josh Lurz
 */

#include "sectors/include/afinal_demand.h"
#include "util/base/include/value.h"
#include "util/base/include/time_vector.h"

// Forward declarations
class Demographic;

/*! 
 * \ingroup Objects
 * \brief A class which represents a single end use of an energy product or
 *        service.
 * \details Energy final demands consume an energy derived good and are counted
 *          towards the total final energy of the region.
 */

class EnergyFinalDemand: public AFinalDemand
{
    friend class XMLDBOutputter;

public:
    static const std::string& getXMLNameStatic();
    
    virtual const std::string& getXMLName() const;

    EnergyFinalDemand();

    virtual ~EnergyFinalDemand();

    virtual void toDebugXML( const int aPeriod,
                             std::ostream& aOut,
                             Tabs* aTabs ) const;
    
    virtual const std::string& getName() const;
    
    virtual void completeInit( const std::string& aRegionName,
                               const IInfo* aRegionInfo );

    virtual void initCalc( const std::string& aRegionName,
                           const Demographic* aDemographics,
                           const int aPeriod );

    virtual void setFinalDemand( const std::string& aRegionName,
                                 const Demographic* aDemographics,
                                 const int aPeriod );

    virtual double getWeightedEnergyPrice( const std::string& aRegionName,
                                           const int aPeriod ) const;

    virtual void accept( IVisitor* aVisitor, const int aPeriod ) const;
protected:

    // TODO: Move all these.
    class IDemandFunction {
    public:
        virtual ~IDemandFunction() {}
        // TODO: Remove this function once construction is cleanly implemented.
        virtual bool isPerCapitaBased() const = 0;

        virtual double calcDemand( const std::string& aRegionName,
                                   const Demographic* aDemographics,
                                   const double aPriceElasticity,
                                   const double aIncomeElasticity,
                                   const double aPriceRatio,
                                   const int aPeriod ) const = 0;
    };

    class PerCapitaGDPDemandFunction: public IDemandFunction {
    public:
        virtual bool isPerCapitaBased() const {
            return true;
        }

        virtual double calcDemand( const std::string& aRegionName,
                                   const Demographic* aDemographics,
                                   const double aPriceElasticity,
                                   const double aIncomeElasticity,
                                   const double aPriceRatio,
                                   const int aPeriod ) const;
    };

    class TotalGDPDemandFunction: public IDemandFunction {
    public:
        virtual bool isPerCapitaBased() const {
            return false;
        }

        virtual double calcDemand( const std::string& aRegionName,
                                   const Demographic* aDemographics,
                                   const double aPriceElasticity,
                                   const double aIncomeElasticity,
                                   const double aPriceRatio,
                                   const int aPeriod ) const;
    };

    // TODO: get rid of this?  Would have to move AEEI out into EnergyFinalDemand.
    class FinalEnergyConsumer {
    public:
        static const std::string& getXMLNameStatic();
        
        const std::string& getXMLName() const;

        static double noCalibrationValue();

        FinalEnergyConsumer();

        void completeInit( const std::string& aRegionName,
                           const std::string& aFinalDemandName );

        double getCalibratedFinalEnergy( const int aPeriod ) const;

        void updateAEEI( const std::string& aRegionName,
                         const int aPeriod );

        double calcTechChange( const int aPeriod ) const;
    
        void toDebugXML( const int aPeriod,
                         std::ostream& aOut,
                         Tabs* aTabs ) const;
    protected:
        
        DEFINE_DATA(
            DEFINE_SUBCLASS_FAMILY( FinalEnergyConsumer ),
            
            //! Name of the TFE market.
            DEFINE_VARIABLE( SIMPLE, "market-name", mTFEMarketName, std::string ),
                    
            //! Autonomous end-use energy intensity parameter.
            DEFINE_VARIABLE( ARRAY, "aeei", mAEEI, objects::PeriodVector<Value> ),
            
            //! Final energy to calibrate to.
            DEFINE_VARIABLE( ARRAY, "cal-final-energy", mCalFinalEnergy, objects::PeriodVector<Value> )
        )
    };
    
    // Define data such that introspection utilities can process the data from this
    // subclass together with the data members of the parent classes.
    DEFINE_DATA_WITH_PARENT(
        AFinalDemand,
    
        //! Name of the final demand and the good it consumes.
        DEFINE_VARIABLE( SIMPLE, "name", mName, std::string ),
                            
        DEFINE_VARIABLE( SIMPLE, "perCapitaBased", mIsPerCapBased, bool ),
        
        //! Total end-use sector service after technical change is applied.
        DEFINE_VARIABLE( ARRAY | STATE | NOT_PARSABLE, "service", mServiceDemands, objects::PeriodVector<Value> ),

        //! Income elasticity 
        DEFINE_VARIABLE( ARRAY, "income-elasticity", mIncomeElasticity, objects::PeriodVector<Value> ),

        //! Price elasticity.
        DEFINE_VARIABLE( ARRAY, "price-elasticity", mPriceElasticity, objects::PeriodVector<Value> ),

        //! Service demand without technical change applied.
        DEFINE_VARIABLE( ARRAY | STATE | NOT_PARSABLE, "service-pre-tech-change", mPreTechChangeServiceDemand, objects::PeriodVector<Value> ),

        //! Per capita service for each period to which to calibrate.
        DEFINE_VARIABLE( ARRAY, "base-service", mBaseService, objects::PeriodVector<Value> ),
                            
        //! Object responsible for consuming final energy.
        DEFINE_VARIABLE( CONTAINER, "final-energy-consumer", mFinalEnergyConsumer, FinalEnergyConsumer* )
    )

    //! Demand function used to calculate unscaled demand.
    std::unique_ptr<IDemandFunction> mDemandFunction;
    
    virtual double calcFinalDemand( const std::string& aRegionName,
                                    const Demographic* aDemographics,
                                    const int aPeriod );

    virtual double calcMacroScaler( const std::string& aRegionName,
                                    const Demographic* aDemographics,
                                    const int aPeriod ) const;

    // Methods for deriving from EnergyFinalDemand.
    virtual void toDebugXMLDerived( const int period, std::ostream& out, Tabs* tabs ) const;
private:    
    void acceptDerived( IVisitor* aVisitor, const int aPeriod ) const;
};

#endif // _ENERGY_FINAL_DEMAND_H_

