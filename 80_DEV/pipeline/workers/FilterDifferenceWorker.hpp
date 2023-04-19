/*****************************************
|      ____    _  _   _   _   ____       |
|     |  _ \  | || | | \ | | |  __|      |
|     | | \ \ | || | |  \| | | |_        |
|     | | | | | || | |     | |  _|       |
|     | |_/ / | || | | |\  | | |__       |
|     |____/  \____/ |_| \_| |____|      |
|                                        |
|   DUNE - Sandbox Depth Visualizer      |
|  A project by                          |
|  Ludwig Zeller and David Schoosleitner |
|                                        |
|****************************************|
|                                        |
| @file FileDifferenceWorker.hpp         |
| @author David Schoosleitner            |
|                                        |
| This file is responsible for:          |
|  - Difference Filter class definition  |
| This file depends on:                  |
|  - Worker superclass                   |     
|  - OpenCV Library                      |
|  - Local Utilities                     |
|                                        |
|****************************************|
|                                        |
| USED LIBRARIES ARE PROPERTY OF THEIR   |
| CORRESPECTIVE OWNERS.                  |
|                                        |
| SEE COPYRIGHT.md IN PROJECT'S ROOT     |
| DIRECTORY, AS WELL AS                  |
| INDIVIDUAL LICENSES AND TOS.           |
|                                        |
*****************************************/

#ifndef __FILTER_DIFFERENCE_WORKER_HPP_
#define __FILTER_DIFFERENCE_WORKER_HPP_


/*****************************************
|               INCLUDES                 |
*****************************************/
#include "../Worker.hpp"


/***********************
 *  NAMESPACE Filter  *
 **********************
 * @namespace Filter
 * @brief Collection of project's filters
***********************/
namespace Filter
{

/***********************
 * CLASS DifferenceWorker
 **********************
 * @class DifferenceWorker : Worker
 * @brief Test Test_c is a class for XYZ.
***********************/
class DifferenceWorker : public Worker
{

private:

    //! (Type: CV_8UC) Storage for first frame
    cv::Mat m_reference;
    //! Flag that triggers a saving action and switches to false upon saving
    bool m_save_flag = true;
    //! Flag for Perlin mode, passes raw input matrix during starting phases
    bool m_do_linger_frames;
    //! Delay statement, will trigger saving and return to regular difference mode upon approaching 0
    int m_linger_ticks = 0;

public:

    /**
     * @fn explicit DifferenceWorker(std::string _id, bool _do_linger_frames = false)
     * @brief Constructor of DifferenceWorker objects. Calls superclass constructor and determines linger frame flag
     * @param _id Worker name-id
     * @param _do_linger_frames Perlin mode flag | true = perlin mode | false = normal mode
     */
    explicit DifferenceWorker(std::string _id, bool _do_linger_frames = false): Worker(std::move(_id), MatIOType::CHAR_8, MatIOType::CHAR_8), m_do_linger_frames(_do_linger_frames)
    { /* No extra construction required */ }

    /**
     * @fn void reset_save()
     * @brief Resets all flags 
     */
    void reset_save()
    {
        this->m_save_flag = true;
        //! Statement irrelevant in normal mode
        if(m_do_linger_frames) 
        {
            m_linger_ticks = PERLIN_LINGER_LENGTH + TEMPORAL_BUFFER_LENGTH;
        }
    }

protected:

    /**
     * @fn void reset_save() override
     * @brief Resets all flags upon pipeline startup
     */
    void start_up() override
    {
        reset_save();
    }

    /**
     * @fn void reset_save() override
     * @brief Cyclic Difference Worker procedure
     */
    void work() override
    {
        //! Submatrix rectangle
        const static cv::Rect frame_rect{CUTOFF_LEFT, CUTOFF_TOP, CUTOFF_RIGHT - CUTOFF_LEFT + 1, CUTOFF_BOT - CUTOFF_TOP + 1};

        //!! Statement to save the first available Frame, has secondary delay statement
        //!! when in "Perlin mode"
        if(m_save_flag && !(m_do_linger_frames && m_linger_ticks >= PERLIN_LINGER_LENGTH))
        {
            this->m_save_flag = false;
            this->m_reference = this->m_work_matrix(frame_rect).clone();
        }

        //!! Statements irrelevant in normal mode
        if(!(--m_linger_ticks))
        {
        //^^

            this->m_work_matrix(frame_rect).forEach<signed char>([&](signed char &value_is, const int *pos)
            {
                value_is = ___min_(14, ___max_(1, 7 - value_is + this->m_reference.at<signed char>(pos[0], pos[1])));
            });
        }
    }

}; //< class DifferenceWorker

}; //< namespace Filter

#endif //< __FILTER_DIFFERENCE_WORKER_HPP_
