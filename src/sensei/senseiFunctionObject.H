/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2018 OpenCFD Ltd.
    Copyright (C) 2018 CINECA
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::functionObjects::senseiFunctionObject

Group
    grpUtilitiesFunctionObjects

Description
    A sensei adaptor for OpenFOAM.

    Example of function object specification:
    \verbatim
    sensei
    {
        type            sensei;
        libs            ("libsenseiFoam.so");
        executeControl  timeStep;
        executeInterval 1;

        scripts         ( ... );

        inputs
        {
            input1
            {
                type    default;
                regions (".*solid" );
                fields  (U p);
            }
            input2
            {
                type    cloud;
                cloud   someCloud;
                fields  ( T U p rho );
            }
        }
    }
    \endverbatim

Usage
    \table
        Property  | Description                  | Required | Default
        type      | sensei                       | yes      |
        log       | report extra information     | no       | false
        scripts   | Sensei pipeline scripts      | yes      |
        inputs    | dictionary of sensei inputs  | yes      |
        outputDir | the output directory         | no       | "\<case\>/insitu"
        mkdir     | optional directory to create | no       |
    \endtable

Note
    The execution frequency can be defined by both the functionObject and
    by the sensei pipeline.

See also
    Foam::functionObjects::functionObject
    Foam::functionObjects::fvMeshFunctionObject
    Foam::functionObjects::timeControl
    Foam::sensei::DataAdaptor
    Foam::sensei::cloudInput
    Foam::sensei::faMeshInput
    Foam::sensei::fvMeshInput

SourceFiles
    senseiFunctionObject.C
    senseiFunctionObjectTemplates.C

\*---------------------------------------------------------------------------*/

#ifndef functionObjects_senseiFunctionObject_H
#define functionObjects_senseiFunctionObject_H

#include "className.H"
#include "wordList.H"
#include "stringList.H"
#include "polyMesh.H"
#include "PtrList.H"
#include "functionObject.H"
#include "senseiCoprocess.H"
#include "senseiInput.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{

/*---------------------------------------------------------------------------*\
                   Class senseiFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class senseiFunctionObject : public functionObject
{
private:

  //- Reference to the time database
  const Time& time_;

  //- The output directory
  fileName outputDir_;

  //- Python scripts for the sensei pipeline
  stringList scripts_;

  //- The sensei coprocess
  autoPtr<sensei::coprocess> adaptor_;

  //- Pointers to the requested sensei inputs
  PtrList<sensei::senseiInput> inputs_;


  // Private Member Functions

  //- No copy construct
  senseiFunctionObject(const senseiFunctionObject&) = delete;

  //- No copy assignment
  void operator=(const senseiFunctionObject&) = delete;


public:

  // Static Methods

  //- Expand strings as filenames, retaining only those that exist
  static label expand(List<string>& scripts, const dictionary& dict);


  //- Runtime type information
  TypeName("sensei");


  // Constructors

  //- Construct from Time and dictionary
  senseiFunctionObject
  (
    const word& name,
    const Time& runTime,
    const dictionary& dict
  );


  //- Destructor
  virtual ~senseiFunctionObject();

  // Member Functions

  //- Read the specification
  virtual bool read(const dictionary& dict);

  //- Execute sensei pipelines
  virtual bool execute();

  //- Write - does nothing
  virtual bool write();

  //- On end - provide feedback about disconnecting sensei.
  virtual bool end();

  //- Update for changes of mesh
  virtual void updateMesh(const mapPolyMesh& mpm);

  //- Update for mesh point-motion
  virtual void movePoints(const polyMesh& mesh);

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace functionObjects
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
