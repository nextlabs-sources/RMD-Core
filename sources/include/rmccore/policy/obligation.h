//
//  obligation.h
//  rmccore
//
//  Created by Haobo Wang on 4/3/18.
//  Copyright © 2018 NextLabs, Inc. All rights reserved.
//

#pragma once
#ifndef __RMCCORE_POLICY_OBLIGATION_H__
#define __RMCCORE_POLICY_OBLIGATION_H__


#include "rmccore/format/nxlpolicy.h"
#include "rmccore/restful/rmrestbase.h"

using namespace RMCCORE::NXLFMT;

namespace RMCCORE {
    namespace OBLIGATION{
        class Watermark : public RestfulBase, public Obligation
        {
        public:
            enum Rotation {
                NOROTATION = 0,
                CLOCKWISE,
                ANTICLOCKWISE
            };

            Watermark();


            Watermark(const std::string &text, const std::string & fontName, const std::string &fontColor, int fontSize, int transparency, Rotation rotation, bool repeat);
            virtual ~Watermark() {}
            
            Watermark& operator = (const Watermark& rhs);
			Watermark& operator = (const Obligation & rhs);

			bool operator== (const Watermark &rhs) const;

        public:
            bool empty() const {return !m_initialized;}
            
            std::string getText() const;
            std::string getFontName() const;
            int getFontSize() const;
            std::string GetFontColor() const;
            int getTransparency() const;
            Rotation getRotation() const;
            std::string getRotationString() const;
            bool getRepeat() const;

		//function inherit from RestfulBase
        public:

            RetValue ImportFromJson(const nlohmann::json& root);

            nlohmann::json ExportToJson();

        private:
            bool    m_initialized;
        };
        
        class WatermarkConfig : public RestfulBase
        {
        public:
            WatermarkConfig();
            ~WatermarkConfig(){};

			bool operator== (const WatermarkConfig &rhs) const;

        public:
            const Watermark & GetWatermarkSetting() const { return m_watermark;}
            const std::string & GetSerialNo() const {return m_serialno;}

		//function inherit from RestfulBase
		public:

            RetValue ImportFromJson(const nlohmann::json& root);

            nlohmann::json ExportToJson();

        private:
            Watermark   m_watermark;
            std::string m_serialno;
        };
    }
}

#endif /* __RMCCORE_POLICY_OBLIGATION_H__ */
