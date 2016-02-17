/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
* Copyright 2015 Pelican Mapping
* http://osgearth.org
*
* osgEarth is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/AnnotationRegistry>
#include <osgEarthAnnotation/AnnotationUtils>
#include <osgEarthFeatures/GeometryCompiler>
#include <osgEarthSymbology/GeometryFactory>
#include <osgEarthSymbology/ExtrusionSymbol>
#include <osgEarth/MapNode>
#include <osgEarth/DrapeableNode>
#include <osg/MatrixTransform>
#include <cmath>

using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;


CircleNode::CircleNode(MapNode*           mapNode,
                       const GeoPoint&    position,
                       const Linear&      radius,
                       const Style&       style,
                       const Angle&       arcStart,
                       const Angle&       arcEnd,
                       const bool         pie):

GeoPositionNode    ( mapNode, position ),
_radius      ( radius ),
_style       ( style ),
_arcStart    ( arcStart ),
_arcEnd      ( arcEnd ),
_pie         ( pie ),
_numSegments ( 0 )
{
    initCircleNode();
}


void
CircleNode::initCircleNode()
{
    rebuild();
}

const Linear&
CircleNode::getRadius() const
{
    return _radius;
}

void
CircleNode::setRadius( const Linear& radius )
{
    if (_radius != radius )
    {
        _radius = radius;
        rebuild();
    }
}

unsigned int
CircleNode::getNumSegments() const
{
    return _numSegments;
}

void
CircleNode::setNumSegments(unsigned int numSegments )
{
    if (_numSegments != numSegments )
    {
        _numSegments = numSegments;
        rebuild();
    }
}

const Style&
CircleNode::getStyle() const
{
    return _style;
}

void
CircleNode::setStyle( const Style& style )
{
    _style = style;
    rebuild();
}

const Angle&
CircleNode::getArcStart(void) const
{
	return (_arcStart);
}

void
CircleNode::setArcStart(const Angle& arcStart)
{
	_arcStart = arcStart;
	rebuild();
}

const Angle&
CircleNode::getArcEnd(void) const
{
	return (_arcEnd);
}

void
CircleNode::setArcEnd(const Angle& arcEnd)
{
	_arcEnd = arcEnd;
	rebuild();
}

const bool&
CircleNode::getPie(void) const
{
	return (_pie);
}

void
CircleNode::setPie(const bool& pie)
{
    _pie = pie;
    rebuild();
}

void
CircleNode::rebuild()
{
    osgEarth::clearChildren( getPositionAttitudeTransform() );

    // construct a local-origin circle.
    GeometryFactory factory;
    Geometry* geom = NULL;
    if (std::abs(_arcEnd.as(Units::DEGREES) - _arcStart.as(Units::DEGREES)) >= 360.0)
    {
        geom = factory.createCircle(osg::Vec3d(0,0,0), _radius, _numSegments);
    }
    else
    {
        geom = factory.createArc(osg::Vec3d(0,0,0), _radius, _arcStart, _arcEnd, _numSegments, 0L, _pie);
    }

    if ( geom )
    {
        GeometryCompiler compiler;
        osg::ref_ptr<osg::Node> node = compiler.compile( geom, _style );
        if ( node.valid() )
        {
            node = AnnotationUtils::installOverlayParent( node.get(), _style );
            getPositionAttitudeTransform()->addChild( node.get() );
        }

        applyRenderSymbology( _style );

        setLightingIfNotSet( false );
    }
}


//-------------------------------------------------------------------

OSGEARTH_REGISTER_ANNOTATION( circle, osgEarth::Annotation::CircleNode );


CircleNode::CircleNode(MapNode*              mapNode,
                       const Config&         conf,
                       const osgDB::Options* dbOptions) :
GeoPositionNode    ( mapNode, conf ),
_radius      ( 1.0, Units::KILOMETERS ),
_numSegments ( 0 )
{
    conf.getObjIfSet( "radius", _radius );
    conf.getObjIfSet( "style",  _style );
    conf.getIfSet   ( "num_segments", _numSegments );
    initCircleNode();
}

Config
CircleNode::getConfig() const
{
    Config conf = GeoPositionNode::getConfig();
    conf.key() = "circle";

    conf.addObj( "radius", _radius );
    conf.addObj( "style",  _style );

    if ( _numSegments != 0 )
        conf.add( "num_segments", _numSegments );

    return conf;
}
