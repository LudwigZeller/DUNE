//
// Created by Ludwig Zeller on 09.08.2022.
//

#ifndef DEPTHCAMERA_FILTER_HPP
#define DEPTHCAMERA_FILTER_HPP

#include <librealsense2/rs.hpp>
using namespace rs2;
    class hole_filling_filter : public filter
    {
    public:
        /**
        * Create hole filling filter
        * The processing performed depends on the selected hole filling mode.
        */
        hole_filling_filter() : filter(init(), 1) {}

        /**
        * Create hole filling filter
        * The processing performed depends on the selected hole filling mode.
        * \param[in] mode - select the hole fill mode:
        * 0 - fill_from_left - Use the value from the left neighbor pixel to fill the hole
        * 1 - farest_from_around - Use the value from the neighboring pixel which is furthest away from the sensor
        * 2 - nearest_from_around - - Use the value from the neighboring pixel closest to the sensor
        */
        hole_filling_filter(int mode) : filter(init(), 1)
        {
            set_option(RS2_OPTION_HOLES_FILL, float(mode));
        }

        hole_filling_filter(filter f) :filter(f)
        {
            rs2_error* e = nullptr;
            if (!rs2_is_processing_block_extendable_to(f.get(), RS2_EXTENSION_HOLE_FILLING_FILTER, &e) && !e)
            {
                _block.reset();
            }
            error::handle(e);
        }
    private:
        friend class context;

        std::shared_ptr<rs2_processing_block> init()
        {
            rs2_error* e = nullptr;
            auto block = std::shared_ptr<rs2_processing_block>(
                    rs2_create_hole_filling_filter_block(&e),
                    rs2_delete_processing_block);
            error::handle(e);

            // Redirect options API to the processing block
            //options::operator=(_block);

            return block;
        }
};


#endif //DEPTHCAMERA_FILTER_HPP
