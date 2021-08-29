import React from "react";
import { AppBar, Typography, Toolbar } from "@material-ui/core";

function CameraPage() {
  return (
    <>
      <AppBar position="static">
        <Toolbar>
          <Typography variant="h6">Manage Camera</Typography>
        </Toolbar>
      </AppBar>
    </>
  );
}

export default CameraPage;
